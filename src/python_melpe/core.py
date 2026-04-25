from __future__ import annotations

from typing import Any

import numpy as np

try:
    import torch
except Exception:  # pragma: no cover - torch is optional at runtime
    torch = None

try:
    from . import _melpe_native
except ImportError as exc:  # pragma: no cover - exercised when extension is not built
    _melpe_native = None
    _NATIVE_IMPORT_ERROR = exc
else:
    _NATIVE_IMPORT_ERROR = None


FRAME_SAMPLES = 540
SAMPLE_RATE_HZ = 8000
ALGORITHMIC_DELAY_SAMPLES = 360
_PCM_SCALE = 32768.0


def is_available() -> bool:
    return _melpe_native is not None


def simulate_melpe(
    audio: Any,
    sample_rate: int = SAMPLE_RATE_HZ,
    compensate_delay: bool = True,
    delay_samples: int = ALGORITHMIC_DELAY_SAMPLES,
) -> Any:
    """Round-trip audio through MELPe and return only the decoded waveform.

    The codec expects 8 kHz PCM audio. The last dimension is treated as time,
    and any leading dimensions are processed independently.

    By default the returned signal is shifted left by the codec's inferred
    fixed algorithmic delay so it stays approximately aligned with the input.
    """

    native = _require_native()

    if sample_rate != SAMPLE_RATE_HZ:
        raise ValueError(
            f"MELPe in this repository expects 8 kHz audio, got sample_rate={sample_rate}."
        )

    if delay_samples < 0:
        raise ValueError(f"delay_samples must be non-negative, got {delay_samples}.")

    if torch is not None and isinstance(audio, torch.Tensor):
        return _simulate_torch(audio, native, compensate_delay, delay_samples)

    return _simulate_numpy(np.asarray(audio), native, compensate_delay, delay_samples)


def _require_native():
    if _melpe_native is None:
        raise ImportError(
            "python_melpe native extension is not built. "
            "Install this package with `python -m pip install .` on a machine "
            "with a working C compiler."
        ) from _NATIVE_IMPORT_ERROR
    return _melpe_native


def _simulate_torch(
    audio: "torch.Tensor", native: Any, compensate_delay: bool, delay_samples: int
) -> "torch.Tensor":
    original_dtype = audio.dtype
    original_device = audio.device

    if audio.dtype == torch.bfloat16:
        numpy_audio = audio.detach().to(dtype=torch.float32).cpu().contiguous().numpy()
    else:
        numpy_audio = audio.detach().cpu().contiguous().numpy()

    output = _simulate_numpy(numpy_audio, native, compensate_delay, delay_samples)
    tensor = torch.from_numpy(output)
    return tensor.to(device=original_device, dtype=original_dtype)


def _simulate_numpy(
    audio: np.ndarray, native: Any, compensate_delay: bool, delay_samples: int
) -> np.ndarray:
    if audio.ndim == 0:
        raise ValueError("audio must have at least one dimension.")

    original_dtype = audio.dtype
    if audio.shape[-1] == 0:
        return np.array(audio, copy=True)

    pcm = _to_pcm16(audio)
    flat = np.ascontiguousarray(pcm).reshape(-1, pcm.shape[-1])
    processed = np.empty_like(flat)

    for index, row in enumerate(flat):
        if compensate_delay:
            raw = native.roundtrip_pcm16_padded(row.tobytes())
            padded = np.frombuffer(raw, dtype=np.int16).copy()
            aligned = _compensate_delay(padded, delay_samples)
            processed[index] = aligned[: row.shape[0]]
        else:
            raw = native.roundtrip_pcm16(row.tobytes())
            processed[index] = np.frombuffer(raw, dtype=np.int16, count=row.shape[0]).copy()

    restored = processed.reshape(pcm.shape)
    return _restore_dtype(restored, original_dtype)


def _compensate_delay(audio: np.ndarray, delay_samples: int) -> np.ndarray:
    if delay_samples == 0:
        return audio

    output = np.zeros_like(audio)
    if delay_samples >= audio.shape[-1]:
        return output

    output[..., :-delay_samples] = audio[..., delay_samples:]
    return output


def _to_pcm16(audio: np.ndarray) -> np.ndarray:
    if audio.dtype.kind == "f":
        float_audio = np.asarray(audio, dtype=np.float32)
        clipped = np.clip(float_audio, -1.0, (32767.0 / _PCM_SCALE))
        return np.rint(clipped * _PCM_SCALE).astype(np.int16)

    if audio.dtype == np.int16:
        return np.ascontiguousarray(audio)

    if audio.dtype.kind == "i":
        return np.clip(audio, -32768, 32767).astype(np.int16)

    raise TypeError(
        "audio must be a floating-point or signed integer NumPy array / PyTorch tensor."
    )


def _restore_dtype(audio: np.ndarray, original_dtype: np.dtype) -> np.ndarray:
    if original_dtype.kind == "f":
        restored = audio.astype(np.float32) / _PCM_SCALE
        return restored.astype(original_dtype, copy=False)

    if original_dtype == np.int16:
        return audio

    if original_dtype.kind == "i":
        info = np.iinfo(original_dtype)
        clipped = np.clip(audio, info.min, info.max)
        return clipped.astype(original_dtype, copy=False)

    raise TypeError(f"Unsupported dtype restoration target: {original_dtype!r}")

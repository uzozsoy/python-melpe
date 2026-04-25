# python-melpe

Cross-platform Python wrapper for applying MELPe 1200 bps compression
artifacts to NumPy arrays and PyTorch tensors.

This repository adapts the GPL-3.0 MELPe codec sources from
[Rhizomatica/melpe](https://github.com/Rhizomatica/melpe) into a small
Python module focused on one job: round-trip audio through the codec and
return only the decoded waveform. It is intended for data augmentation,
artifact simulation, and offline experiments where you want MELPe-style
degradation without separately handling encoded bitstreams.

## Features

- NumPy and PyTorch input support
- Same-shape output, with the last axis treated as time
- 8 kHz audio processing
- Fixed delay compensation enabled by default
- Native extension buildable on both Windows and Ubuntu

## Installation

```bash
python -m pip install .
```

For editable development installs:

```bash
python -m pip install -e .
```

### Build requirements

The package builds a CPython native extension from the MELPe C sources.

- Ubuntu: `build-essential` and Python development headers
- Windows: Visual Studio Build Tools with the C++ workload

Install name: `python-melpe`

Import name: `python_melpe`

## Usage

```python
import numpy as np
import python_melpe as melpe

audio = np.random.randn(8000).astype(np.float32) * 0.05
artifacted = melpe.simulate_melpe(audio, sample_rate=8000)
```

With PyTorch:

```python
import torch
import python_melpe as melpe

audio = torch.randn(2, 8000, dtype=torch.float32) * 0.05
artifacted = melpe.simulate_melpe(audio, sample_rate=8000)
```

To disable delay compensation and get the raw decoded timing:

```python
artifacted = melpe.simulate_melpe(
    audio,
    sample_rate=8000,
    compensate_delay=False,
)
```

## API

### `simulate_melpe(audio, sample_rate=8000, compensate_delay=True, delay_samples=360)`

Round-trip audio through MELPe and return only the decoded waveform.

- `audio`: `numpy.ndarray` or `torch.Tensor`
- `sample_rate`: must be `8000`
- `compensate_delay`: shifts the decoded output left by the codec's fixed
  inferred algorithmic delay
- `delay_samples`: override for the default 360-sample delay compensation

Other exported helpers:

- `python_melpe.is_available()`
- `python_melpe.FRAME_SAMPLES`
- `python_melpe.SAMPLE_RATE_HZ`
- `python_melpe.ALGORITHMIC_DELAY_SAMPLES`

## Implementation Notes

- The upstream codec is frame-based and operates on 540-sample frames at
  8 kHz.
- This wrapper does not expose the encoded MELPe bitstream. It only
  returns the decoded waveform after a codec round-trip.
- Delay compensation is applied to the padded decoded output first, then
  the signal is trimmed back to the original length. This avoids adding an
  unnecessary silent tail during alignment.
- The upstream C codec uses global state, so calls are intentionally
  serialized by the Python GIL.

## Attribution and License

This project is built directly on the MELPe codec sources from
[Rhizomatica/melpe](https://github.com/Rhizomatica/melpe). The original
codec sources are GPL-3.0 licensed, and the wrapper in this repository is
distributed under the same license.

If you redistribute or publish this project, keep the existing license and
attribution intact.

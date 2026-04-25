#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <limits.h>
#include <stdint.h>
#include <string.h>

#include "melpe/melpe.h"

#if USHRT_MAX != 65535
#error "This wrapper requires 16-bit short support."
#endif

#define MELPE_FRAME_SAMPLES 540
#define MELPE_PACKET_BYTES 11

static PyObject *roundtrip_pcm16_impl(PyObject *self, PyObject *args, int keep_padding)
{
    Py_buffer input = {0};
    PyObject *output = NULL;
    char *output_buffer = NULL;
    const char *input_bytes = NULL;
    Py_ssize_t total_samples = 0;
    Py_ssize_t output_samples = 0;
    Py_ssize_t offset = 0;

    int16_t frame_in[MELPE_FRAME_SAMPLES];
    int16_t frame_out[MELPE_FRAME_SAMPLES];
    unsigned char encoded[MELPE_PACKET_BYTES];

    (void)self;

    if (!PyArg_ParseTuple(args, "y*", &input)) {
        return NULL;
    }

    if (input.len % (Py_ssize_t)sizeof(int16_t) != 0) {
        PyBuffer_Release(&input);
        PyErr_SetString(PyExc_ValueError, "PCM buffer length must be a multiple of 2 bytes.");
        return NULL;
    }

    if (input.len == 0) {
        PyBuffer_Release(&input);
        return PyBytes_FromStringAndSize("", 0);
    }

    total_samples = input.len / (Py_ssize_t)sizeof(int16_t);
    output_samples =
        keep_padding
            ? ((total_samples + MELPE_FRAME_SAMPLES - 1) / MELPE_FRAME_SAMPLES) *
                  MELPE_FRAME_SAMPLES
            : total_samples;

    output = PyBytes_FromStringAndSize(
        NULL, output_samples * (Py_ssize_t)sizeof(int16_t));
    if (output == NULL) {
        PyBuffer_Release(&input);
        return NULL;
    }

    output_buffer = PyBytes_AS_STRING(output);
    input_bytes = (const char *)input.buf;

    melpe_i();

    for (offset = 0; offset < output_samples; offset += MELPE_FRAME_SAMPLES) {
        Py_ssize_t chunk_samples = total_samples - offset;
        if (chunk_samples > MELPE_FRAME_SAMPLES) {
            chunk_samples = MELPE_FRAME_SAMPLES;
        }
        if (chunk_samples < 0) {
            chunk_samples = 0;
        }

        memset(frame_in, 0, sizeof(frame_in));
        if (chunk_samples > 0) {
            memcpy(frame_in, input_bytes + (offset * (Py_ssize_t)sizeof(int16_t)),
                   (size_t)chunk_samples * sizeof(int16_t));
        }

        melpe_a(encoded, (short *)frame_in);
        melpe_s((short *)frame_out, encoded);

        memcpy(output_buffer + (offset * (Py_ssize_t)sizeof(int16_t)), frame_out,
               (size_t)(keep_padding ? MELPE_FRAME_SAMPLES : chunk_samples) *
                   sizeof(int16_t));
    }

    PyBuffer_Release(&input);
    return output;
}

static PyObject *roundtrip_pcm16(PyObject *self, PyObject *args)
{
    return roundtrip_pcm16_impl(self, args, 0);
}

static PyObject *roundtrip_pcm16_padded(PyObject *self, PyObject *args)
{
    return roundtrip_pcm16_impl(self, args, 1);
}

static PyMethodDef module_methods[] = {
    {"roundtrip_pcm16", roundtrip_pcm16, METH_VARARGS,
     "Round-trip little-endian PCM16 mono samples through MELPe."},
    {"roundtrip_pcm16_padded", roundtrip_pcm16_padded, METH_VARARGS,
     "Round-trip PCM16 samples through MELPe and keep the padded decoded tail."},
    {NULL, NULL, 0, NULL},
};

static struct PyModuleDef module_definition = {
    PyModuleDef_HEAD_INIT,
    "_melpe_native",
    "Native MELPe PCM16 round-trip helper.",
    -1,
    module_methods,
};

PyMODINIT_FUNC PyInit__melpe_native(void)
{
    PyObject *module = PyModule_Create(&module_definition);
    if (module == NULL) {
        return NULL;
    }

    if (PyModule_AddIntConstant(module, "FRAME_SAMPLES", MELPE_FRAME_SAMPLES) < 0) {
        Py_DECREF(module);
        return NULL;
    }

    if (PyModule_AddIntConstant(module, "SAMPLE_RATE_HZ", 8000) < 0) {
        Py_DECREF(module);
        return NULL;
    }

    return module;
}

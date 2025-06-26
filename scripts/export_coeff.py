import pywt
import sys
from pathlib import Path
from PIL import Image
import matplotlib.pyplot as plt

import matplotlib as mpl
mpl.font_manager._rebuild()

HDR_FILE_TEMPLATE="""\
/*
  Wavelet coefficients header
  
  Kindly borrowed from pywavlets, thanks!
  
  https://github.com/PyWavelets/pywt
*/
#pragma once

struct WaveletCoeffs
{{
    const char * const name;
    const unsigned int length;
    const float* dc_lo;
    const float* dc_hi;
    const float* rc_lo;
    const float* rc_hi; 
}};

{wavelet_externs}

#define WAVELET_COUNT {wavelet_count}

extern const WaveletCoeffs * const wavelet_coeffs[{wavelet_count}];
"""

SRC_FILE_TEMPLATE = """\
/*
  Wavelet coefficients sources

  Kindly borrowed from pywavlets, thanks!

  https://github.com/PyWavelets/pywt
*/
#include "wavelet_coeffs.hpp"

{wavelet_structs}

const WaveletCoeffs * const wavelet_coeffs[{wavelet_count}] = {{
    {wavelet_pointers}
}};
"""

SRC_WAVELET_TEMPLATE = """\
// -----------------------------------------------------------------------------------------------
// {family_name} {family_number}
// -----------------------------------------------------------------------------------------------
static float {q_name}_dc_lo[{length}] = {{{dc_lo}}};

static float {q_name}_dc_hi[{length}] = {{{dc_hi}}};

static float {q_name}_rc_lo[{length}] = {{{rc_lo}}};

static float {q_name}_rc_hi[{length}] = {{{rc_hi}}};

const WaveletCoeffs {q_name}_coeffs = {{
    "{name}", 
    {length},
    {q_name}_dc_lo,
    {q_name}_dc_hi,
    {q_name}_rc_lo,
    {q_name}_rc_hi
}};
"""

def fmt_coeffs(coeffs):

    scoeffs = [f"{c:1.12f}f" for c in coeffs]

    if len(coeffs) <= 4:
        return f" {', '.join(scoeffs)} "
    else:
        split_lines = ",\n        ".join([", ".join(scoeffs[i:i+4]) for i in range(0, len(scoeffs), 4)])
        return f"\n        {split_lines}\n"


def fig2img(fig):
    """Convert a Matplotlib figure to a PIL Image and return it"""
    import io
    buf = io.BytesIO()
    fig.savefig(buf)
    buf.seek(0)
    img = Image.open(buf)
    return img
def wavelet_to_image(wl):
    waves = wl.wavefun(8)

    DARK_BLUE = "#003566"
    LITE_BLUE ="#0080c4"

    fig, axs = plt.subplots(1, 4, facecolor=DARK_BLUE)
    fig.set_size_inches(2, 0.6)

    if len(waves) == 5:
        shapes = (
            waves[0], waves[1],waves[2], waves[3]
        )
    else:
        shapes = (
            waves[0], waves[1],waves[0], waves[1]
        )

    for s, ax in zip(shapes, axs):
        ax.plot(waves[-1], s, c=LITE_BLUE, lw=2)
        ax.plot(waves[-1], s, c="white", lw=0.5)
        ax.spines['top'].set_visible(False)
        ax.spines['right'].set_visible(False)
        ax.spines['bottom'].set_visible(False)
        ax.spines['left'].set_visible(False)
        ax.axis('off')

    plt.tight_layout(pad=0, w_pad=0, h_pad=0)
    plt.subplots_adjust(top=0.75, left=0.02, right=0.98, bottom=0.1)

    sfnlen = len(wl.short_family_name)
    if len(wl.name) > sfnlen:
        number = wl.name[sfnlen:]
        s = f"{wl.family_name} {number}"
    else:
        s = f"{wl.family_name}"

    plt.figtext(0.02, 0.95 , s, va='top', color='white', alpha=0.8, fontdict={"family" : "PASTI", "size": 8})
    img = fig2img(fig)
    #img.show()
#    sys.exit(0)
    plt.close(fig)

    return img


if __name__ == "__main__":

    if len(sys.argv) != 3:
        print("export_coeff.py [wavelet_output_dir] [image_file]")
        sys.exit(1)

    base = Path(sys.argv[1])
    image_file = Path(sys.argv[2])

    names = pywt.wavelist(family=None, kind='discrete')

    wavelet_structs = []
    wavelet_pointers = []
    wavelet_externs = []

    large_image = None

    for i, name in enumerate(names):
        wl = pywt.Wavelet(name)
        q_name = name.replace(".", "_")

        img = wavelet_to_image(wl)

        if i == 0:
            large_image = Image.new('RGB', (img.width, img.height * len(names) ))

        large_image.paste(img, (0, img.height * i))

        template_params = {
            'q_name': q_name,
            'name': name,
            'length': len(wl.dec_lo),
            'family_name': wl.family_name,
            'family_number': wl.family_number,
            'dc_lo': fmt_coeffs(wl.dec_lo),
            'dc_hi': fmt_coeffs(wl.dec_hi),
            'rc_lo': fmt_coeffs(wl.rec_lo),
            'rc_hi': fmt_coeffs(wl.rec_hi),
        }
        wavelet_structs.append(SRC_WAVELET_TEMPLATE.format(**template_params))
        wavelet_pointers.append(f"&{q_name}_coeffs")
        wavelet_externs.append(f"extern const WaveletCoeffs {q_name}_coeffs;")

    wavelet_structs = "\n".join(wavelet_structs)
    wavelet_pointers = ",\n    ".join(wavelet_pointers)

    wavelet_externs = "\n".join(wavelet_externs)

    (base / "wavelet_coeffs.cpp").write_text(
        SRC_FILE_TEMPLATE.format(**{
            'wavelet_structs': wavelet_structs,
            'wavelet_pointers': wavelet_pointers,
            'wavelet_count': len(names)
        })
    )

    (base / "wavelet_coeffs.hpp").write_text(
        HDR_FILE_TEMPLATE.format(**{
            'wavelet_externs': wavelet_externs,
            'wavelet_count': len(names)
        })
    )

    image_file.parent.mkdir(exist_ok=True)

    with image_file.open("wb") as fp:
        large_image.save(fp, "png")

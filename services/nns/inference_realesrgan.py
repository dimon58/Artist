import argparse
import cv2
import glob
import os
from basicsr.archs.rrdbnet_arch import RRDBNet
from realesrgan import RealESRGANer
from realesrgan.archs.srvgg_arch import SRVGGNetCompact
import urllib.request
import urllib.error
from pathlib import Path

file_path = Path(__file__).parent
os.makedirs(os.path.join(file_path,  'pretrained_models'), exist_ok=True)


def chose_model(model_name):
    # determine models according to model names
    model_name = model_name.split('.')[0]
    if model_name in ['RealESRGAN_x4plus', 'RealESRNet_x4plus']:  # x4 RRDBNet model
        model = RRDBNet(num_in_ch=3, num_out_ch=3, num_feat=64, num_block=23, num_grow_ch=32, scale=4)
        netscale = 4
        url = 'https://github.com/xinntao/Real-ESRGAN/releases/download/v0.1.1/RealESRNet_x4plus.pth'

    elif model_name in ['RealESRGAN_x4plus_anime_6B', 'RealESRGAN_x4plus_anime']:  # x4 RRDBNet model with 6 blocks
        model = RRDBNet(num_in_ch=3, num_out_ch=3, num_feat=64, num_block=6, num_grow_ch=32, scale=4)
        netscale = 4
        url = 'https://github.com/xinntao/Real-ESRGAN/releases/download/v0.2.2.4/RealESRGAN_x4plus_anime_6B.pth'

    elif model_name in ['RealESRGAN_x2plus']:  # x2 RRDBNet model
        model = RRDBNet(num_in_ch=3, num_out_ch=3, num_feat=64, num_block=23, num_grow_ch=32, scale=2)
        netscale = 2
        url = 'https://github.com/xinntao/Real-ESRGAN/releases/download/v0.2.1/RealESRGAN_x2plus.pth'

    elif model_name in [
        'RealESRGANv2-anime-xsx2', 'RealESRGANv2-animevideo-xsx2-nousm', 'RealESRGANv2-animevideo-xsx2'
    ]:  # x2 VGG-style model (XS size)
        model = SRVGGNetCompact(num_in_ch=3, num_out_ch=3, num_feat=64, num_conv=16, upscale=2, act_type='prelu')
        netscale = 2
        url = 'https://github.com/xinntao/Real-ESRGAN/releases/download/v0.2.3.0/RealESRGANv2-animevideo-xsx2.pth'

    elif model_name in [
        'RealESRGANv2-anime-xsx4', 'RealESRGANv2-animevideo-xsx4-nousm', 'RealESRGANv2-animevideo-xsx4'
    ]:  # x4 VGG-style model (XS size)
        model = SRVGGNetCompact(num_in_ch=3, num_out_ch=3, num_feat=64, num_conv=16, upscale=4, act_type='prelu')
        netscale = 4
        url = 'https://github.com/xinntao/Real-ESRGAN/releases/download/v0.2.3.0/RealESRGANv2-animevideo-xsx4.pth'

    else:
        raise ValueError(f'Model {model_name} does not exist.')

    return model, netscale, url


def determine_model_paths(model_name, url):
    model_file = url.split('/')[-1]
    model_path = os.path.join(file_path, 'pretrained_models', model_file)
    if not os.path.isfile(model_path):
        try:
            print(f'Downloading model {model_name}')
            urllib.request.urlretrieve(url, os.path.join(file_path, 'pretrained_models', model_file))
        except urllib.error.HTTPError:
            raise ValueError(f'Model {model_name} does not exist.')

    return model_path


def upscale(
        input_file: str = 'inputs',
        output_folder: str = 'outputs',
        model_name: str = 'RealESRGAN_x4plus',
        outscale: float = 4,
        prefix: str = '',
        suffix: str = '_out',
        tile: int = 0,
        tile_pad: int = 10,
        pre_pad: int = 0,
        alpha_upsampler: str = 'realesrgan',
        ext: str = 'auto',
        face_enhance=False,
        half=False,
):
    """
    :param input_file: Input image or folder
    :param output_folder: Output folder
    :param model_name: Model names: RealESRGAN_x4plus | RealESRNet_x4plus | RealESRGAN_x4plus_anime_6B | RealESRGAN_x2plus'
              'RealESRGANv2-anime-xsx2 | RealESRGANv2-animevideo-xsx2-nousm | RealESRGANv2-animevideo-xsx2'
              'RealESRGANv2-anime-xsx4 | RealESRGANv2-animevideo-xsx4-nousm | RealESRGANv2-animevideo-xsx4
    :param outscale: The final upsampling scale of the image
    :param prefix: Prefix of the restored image
    :param suffix: Suffix of the restored image
    :param tile: Tile size, 0 for no tile during testing
    :param tile_pad: Tile padding
    :param pre_pad: Pre padding size at each border
    :param face_enhance: Use GFPGAN to enhance face
    :param half: Use half precision during inference
    :param alpha_upsampler: The upsampler for the alpha channels. Options: realesrgan | bicubic
    :param ext: Image extension. Options: auto | jpg | png, auto means using the same extension as inputs
    """

    # determine model paths
    model, netscale, url = chose_model(model_name)
    model_path = determine_model_paths(model_name, url)

    # restorer
    upsampler = RealESRGANer(
        scale=netscale,
        model_path=model_path,
        model=model,
        tile=tile,
        tile_pad=tile_pad,
        pre_pad=pre_pad,
        half=half)

    if face_enhance:  # Use GFPGAN for face enhancement
        from gfpgan import GFPGANer
        face_enhancer = GFPGANer(
            model_path='https://github.com/TencentARC/GFPGAN/releases/download/v0.2.0/GFPGANCleanv1-NoCE-C2.pth',
            upscale=outscale,
            arch='clean',
            channel_multiplier=2,
            bg_upsampler=upsampler)
    os.makedirs(output_folder, exist_ok=True)

    if os.path.isfile(input_file):
        paths = [input_file]
    else:
        paths = sorted(glob.glob(os.path.join(input_file, '*')))

    for idx, path in enumerate(paths):
        imgname, extension = os.path.splitext(os.path.basename(path))

        img = cv2.imread(path, cv2.IMREAD_UNCHANGED)
        if len(img.shape) == 3 and img.shape[2] == 4:
            img_mode = 'RGBA'
        else:
            img_mode = None

        try:
            if face_enhance:
                _, _, output = face_enhancer.enhance(img, has_aligned=False, only_center_face=False, paste_back=True)
            else:
                output, _ = upsampler.enhance(img, outscale=outscale)
        except RuntimeError as error:
            print('Error', error)
            print('If you encounter CUDA out of memory, try to set --tile with a smaller number.')
        else:
            if ext == 'auto':
                extension = extension[1:]
            else:
                extension = ext
            if img_mode == 'RGBA':  # RGBA images should be saved in png format
                extension = 'png'

            save_path = os.path.join(output_folder, f'{prefix}{imgname}{suffix}.{extension}')
            cv2.imwrite(save_path, output)


def main():
    """
    Inference demo for Real-ESRGAN.
    """
    parser = argparse.ArgumentParser()
    parser.add_argument('-i', '--input', type=str, default='inputs', help='Input image or folder')
    parser.add_argument(
        '-n',
        '--model_name',
        type=str,
        default='RealESRGAN_x4plus',
        help=('Model names: RealESRGAN_x4plus | RealESRNet_x4plus | RealESRGAN_x4plus_anime_6B | RealESRGAN_x2plus'
              'RealESRGANv2-anime-xsx2 | RealESRGANv2-animevideo-xsx2-nousm | RealESRGANv2-animevideo-xsx2'
              'RealESRGANv2-anime-xsx4 | RealESRGANv2-animevideo-xsx4-nousm | RealESRGANv2-animevideo-xsx4'))
    parser.add_argument('-o', '--output', type=str, default='outputs', help='Output folder')
    parser.add_argument('-s', '--outscale', type=float, default=4, help='The final upsampling scale of the image')
    parser.add_argument('--prefix', type=str, default='', help='Prifix of the restored image')
    parser.add_argument('--suffix', type=str, default='_out', help='Suffix of the restored image')
    parser.add_argument('-t', '--tile', type=int, default=0, help='Tile size, 0 for no tile during testing')
    parser.add_argument('--tile_pad', type=int, default=10, help='Tile padding')
    parser.add_argument('--pre_pad', type=int, default=0, help='Pre padding size at each border')
    parser.add_argument('--face_enhance', action='store_true', help='Use GFPGAN to enhance face')
    parser.add_argument('--half', action='store_true', help='Use half precision during inference')
    parser.add_argument(
        '--alpha_upsampler',
        type=str,
        default='realesrgan',
        help='The upsampler for the alpha channels. Options: realesrgan | bicubic')
    parser.add_argument(
        '--ext',
        type=str,
        default='auto',
        help='Image extension. Options: auto | jpg | png, auto means using the same extension as inputs')
    args = parser.parse_args()

    upscale(
        input_file=args.input,
        output_folder=args.output,
        model_name=args.model_name,
        outscale=args.outscale,
        prefix=args.prefix,
        suffix=args.suffix,
        tile=args.tile,
        tile_pad=args.tile_pad,
        pre_pad=args.pre_pad,
        alpha_upsampler=args.alpha_upsampler,
        ext=args.ext,
        face_enhance=args.face_enhance,
        half=args.half,
    )


if __name__ == '__main__':
    main()

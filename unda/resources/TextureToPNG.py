from pathlib import Path
import cv2


for mtl_file_path in Path('./').glob('*.mtl'):
    mtl_file = open(str(mtl_file_path), 'r').readlines()
    for i, line in enumerate(mtl_file):
        key_value = line.split(' ')
        if 'map_Kd' in key_value[0]:
            image_path = Path(key_value[1].rstrip('\n'))
            image_path = Path('./') / image_path
            print(str(image_path))
            tif = cv2.imread(str(image_path), cv2.IMREAD_UNCHANGED)
            image_path = image_path.with_suffix('.png')
            if not image_path.exists():
                cv2.imwrite(str(image_path), tif)

            key_value[1] = image_path.as_posix()
            mtl_file[i] = ' '.join(key_value)
    with open(str(mtl_file_path), 'w') as f:
        f.writelines(mtl_file)

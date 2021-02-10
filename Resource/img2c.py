import sys
from pathlib import Path
from PIL import Image

spriteList = [
  {
    'file':'String.png',
    'name':'SPECIAL_DISPALY_OFF',
    'x':0,
    'y':0,
    'w':1,
    'h':1
  },
  {
    'file':'String.png',
    'name':'SPECIAL_DISPALY_ON',
    'x':0,
    'y':0,
    'w':1,
    'h':1
  },
  {
    'file':'String.png',
    'name':'BLACK',
    'x':0,
    'y':200,
    'w':240,
    'h':10
  },
  {
    'file':'String.png',
    'name':'num0',
    'x':1,
    'y':1,
    'w':18,
    'h':26
  },
  {
    'file':'String.png',
    'name':'num1',
    'x':21,
    'y':1,
    'w':18,
    'h':26
  },
  {
    'file':'String.png',
    'name':'num2',
    'x':41,
    'y':1,
    'w':18,
    'h':26
  },
  {
    'file':'String.png',
    'name':'num3',
    'x':61,
    'y':1,
    'w':18,
    'h':26
  },
  {
    'file':'String.png',
    'name':'num4',
    'x':81,
    'y':1,
    'w':18,
    'h':26
  },
  {
    'file':'String.png',
    'name':'num5',
    'x':101,
    'y':1,
    'w':18,
    'h':26
  },
  {
    'file':'String.png',
    'name':'num6',
    'x':121,
    'y':1,
    'w':18,
    'h':26
  },
  {
    'file':'String.png',
    'name':'num7',
    'x':141,
    'y':1,
    'w':18,
    'h':26
  },
  {
    'file':'String.png',
    'name':'num8',
    'x':161,
    'y':1,
    'w':18,
    'h':26
  },
  {
    'file':'String.png',
    'name':'num9',
    'x':181,
    'y':1,
    'w':18,
    'h':26
  },
  {
    'file':'String.png',
    'name':'numBlack',
    'x':221,
    'y':1,
    'w':18,
    'h':26
  },
  {
    'file':'String.png',
    'name':'minus',
    'x':201,
    'y':1,
    'w':18,
    'h':26
  },
  {
    'file':'main1.png',
    'name':'IconTemp',
    'x':22,
    'y':5,
    'w':18,
    'h':50
  },
  {
    'file':'main1.png',
    'name':'IconHumi',
    'x':22,
    'y':65,
    'w':18,
    'h':50
  },
  {
    'file':'main1.png',
    'name':'IconPress1',
    'x':10,
    'y':125,
    'w':10,
    'h':30
  },
  {
    'file':'main1.png',
    'name':'IconPress2',
    'x':25,
    'y':125,
    'w':10,
    'h':30
  },
  {
    'file':'main1.png',
    'name':'IconPress3',
    'x':40,
    'y':125,
    'w':10,
    'h':30
  },
  {
    'file':'main1.png',
    'name':'IconPress4',
    'x':5,
    'y':168,
    'w':50,
    'h':7
  },
  {
    'file':'main1.png',
    'name':'IconCo2',
    'x':8,
    'y':193,
    'w':43,
    'h':35
  },
  {
    'file':'main1.png',
    'name':'UnitTemp',
    'x':189,
    'y':24,
    'w':25,
    'h':19
  },
  {
    'file':'main1.png',
    'name':'UnitHumi',
    'x':189,
    'y':84,
    'w':22,
    'h':19
  },
  {
    'file':'main1.png',
    'name':'UnitPress',
    'x':189,
    'y':144,
    'w':46,
    'h':19
  },
  {
    'file':'main1.png',
    'name':'UnitCo2',
    'x':189,
    'y':207,
    'w':46,
    'h':16
  },
  {
    'file':'main1.png',
    'name':'DotTemp',
    'x':152,
    'y':37,
    'w':6,
    'h':6
  },
  {
    'file':'main1.png',
    'name':'DotHumi',
    'x':152,
    'y':97,
    'w':6,
    'h':6
  },
  {
    'file':'main1.png',
    'name':'DotPress',
    'x':152,
    'y':157,
    'w':6,
    'h':6
  }
]

# スプライトの型定義
def_struct = '''typedef struct _sprite {
  const uint16_t* image;
  uint16_t image_length;
  uint8_t x;
  uint8_t y;
  uint8_t w;
  uint8_t h;
} sprite_t;'''

# 指定座標の色取得
def getColor(im, x, y):
  r, g, b, a = im.getpixel((x, y))
  
  r = int(r / 8) & 0x1F # 赤は5bit
  g = int(g / 4) & 0x3F # 緑は6bit
  b = int(b / 8) & 0x1F # 青は5bit
  return r<<3 | (g&0b111)<<13 | g>>3 | b<<8

# メイン
if __name__ == '__main__':
  # spriteListに従って画像から色情報を取得
  for sprite in spriteList:
    with Image.open(sprite['file']) as im:
      sprite.update({'img':[]})
      for y in range(sprite['y'], sprite['y']+sprite['h']):
        for x in range(sprite['x'], sprite['x']+sprite['w']):
          sprite['img'].append(getColor(im, x, y))
  
  # 色情報を圧縮
  for sprite in spriteList:
    sprite.update({'zip':[]})
    current_color = sprite['img'][0]
    color_continue = 0
    for color in sprite['img']:
      if color == current_color:
        # 同じ色が連続
        color_continue += 1
      else:
        # 違う色に変わった
        sprite['zip'].append(color_continue)
        sprite['zip'].append(current_color)
        color_continue = 1
        current_color = color
    # 最後の色
    sprite['zip'].append(color_continue)
    sprite['zip'].append(color)
  
  # ヘッダファイル作成
  with open('resource.h', 'w') as hfile:
    hfile.write('\n')
    hfile.write('#ifndef RESOURCE_H_\n')
    hfile.write('#define RESOURCE_H_\n')
    hfile.write('\n')
    hfile.write(def_struct)
    hfile.write('\n\n')
    hfile.write('enum {\n')
    for sprite in spriteList:
      hfile.write('  SPRITE_' + sprite['name'] + ',\n')
    hfile.write('  SPRITE_NUM\n')
    hfile.write('};\n')
    hfile.write('\n')
    hfile.write('extern const sprite_t SpriteList[];\n')
    hfile.write('\n')
    hfile.write('#ifdef RESOURCE\n')
    hfile.write('#undef RESOURCE\n')
    hfile.write('\n')
    for sprite in spriteList:
      hfile.write('const uint16_t ' + sprite['name'] + '[] = {')
      hfile.write(', '.join(map(str, sprite['zip'])))
      hfile.write('};\n')
    hfile.write('\n\n')
    hfile.write('const sprite_t SpriteList[] = {\n')
    total_size = 0
    for sprite in spriteList:
      hfile.write('  {\n')
      hfile.write('    ' + sprite['name'] + ',\n')
      hfile.write('    ' + str(len(sprite['zip'])) + ',\n')
      hfile.write('    ' + str(sprite['x']) + ',\n')
      hfile.write('    ' + str(sprite['y']) + ',\n')
      hfile.write('    ' + str(sprite['w']) + ',\n')
      hfile.write('    ' + str(sprite['h']) + ',\n')
      hfile.write('  },\n')
      total_size += len(sprite['zip']) * 2
      print('name:' + sprite['name'] + ' size: ' + str(len(sprite['zip']) * 2))
    print('totla: ' + str(total_size))
    hfile.write('};\n')
    hfile.write('#endif\n')
    hfile.write('#endif\n')
    hfile.write('\n')

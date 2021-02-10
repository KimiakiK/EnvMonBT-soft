import sys
from pathlib import Path
from PIL import Image

spriteList = [
  { # 特殊スプライト
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
  { # 画面クリア用
    'file':'String.png',
    'name':'BLACK',
    'x':0,
    'y':200,
    'w':240,
    'h':10
  },
  { # 文字(大)
    'file':'String.png',
    'name':'num0_L',
    'x':1,
    'y':1,
    'w':18,
    'h':26
  },
  {
    'file':'String.png',
    'name':'num1_L',
    'x':21,
    'y':1,
    'w':18,
    'h':26
  },
  {
    'file':'String.png',
    'name':'num2_L',
    'x':41,
    'y':1,
    'w':18,
    'h':26
  },
  {
    'file':'String.png',
    'name':'num3_L',
    'x':61,
    'y':1,
    'w':18,
    'h':26
  },
  {
    'file':'String.png',
    'name':'num4_L',
    'x':81,
    'y':1,
    'w':18,
    'h':26
  },
  {
    'file':'String.png',
    'name':'num5_L',
    'x':101,
    'y':1,
    'w':18,
    'h':26
  },
  {
    'file':'String.png',
    'name':'num6_L',
    'x':121,
    'y':1,
    'w':18,
    'h':26
  },
  {
    'file':'String.png',
    'name':'num7_L',
    'x':141,
    'y':1,
    'w':18,
    'h':26
  },
  {
    'file':'String.png',
    'name':'num8_L',
    'x':161,
    'y':1,
    'w':18,
    'h':26
  },
  {
    'file':'String.png',
    'name':'num9_L',
    'x':181,
    'y':1,
    'w':18,
    'h':26
  },
  {
    'file':'String.png',
    'name':'minus_L',
    'x':201,
    'y':1,
    'w':18,
    'h':26
  },
  {
    'file':'String.png',
    'name':'numBlack_L',
    'x':221,
    'y':1,
    'w':18,
    'h':26
  },
  {
    'file':'String.png',
    'name':'dot_L',
    'x':1,
    'y':74,
    'w':6,
    'h':26
  },
  { # 文字(中)
    'file':'String.png',
    'name':'num0_M',
    'x':2,
    'y':29,
    'w':14,
    'h':20
  },
  {
    'file':'String.png',
    'name':'num1_M',
    'x':17,
    'y':29,
    'w':14,
    'h':20
  },
  {
    'file':'String.png',
    'name':'num2_M',
    'x':32,
    'y':29,
    'w':14,
    'h':20
  },
  {
    'file':'String.png',
    'name':'num3_M',
    'x':47,
    'y':29,
    'w':14,
    'h':20
  },
  {
    'file':'String.png',
    'name':'num4_M',
    'x':62,
    'y':29,
    'w':14,
    'h':20
  },
  {
    'file':'String.png',
    'name':'num5_M',
    'x':77,
    'y':29,
    'w':14,
    'h':20
  },
  {
    'file':'String.png',
    'name':'num6_M',
    'x':92,
    'y':29,
    'w':14,
    'h':20
  },
  {
    'file':'String.png',
    'name':'num7_M',
    'x':107,
    'y':29,
    'w':14,
    'h':20
  },
  {
    'file':'String.png',
    'name':'num8_M',
    'x':122,
    'y':29,
    'w':14,
    'h':20
  },
  {
    'file':'String.png',
    'name':'num9_M',
    'x':137,
    'y':29,
    'w':14,
    'h':20
  },
  {
    'file':'String.png',
    'name':'minus_M',
    'x':157,
    'y':29,
    'w':14,
    'h':20
  },
  {
    'file':'String.png',
    'name':'numBlack_M',
    'x':172,
    'y':29,
    'w':14,
    'h':20
  },
  {
    'file':'String.png',
    'name':'dot_M',
    'x':8,
    'y':80,
    'w':5,
    'h':20
  },
  { # 文字(小)
    'file':'String.png',
    'name':'num0_S',
    'x':1,
    'y':52,
    'w':10,
    'h':14
  },
  {
    'file':'String.png',
    'name':'num1_S',
    'x':12,
    'y':52,
    'w':10,
    'h':14
  },
  {
    'file':'String.png',
    'name':'num2_S',
    'x':23,
    'y':52,
    'w':10,
    'h':14
  },
  {
    'file':'String.png',
    'name':'num3_S',
    'x':34,
    'y':52,
    'w':10,
    'h':14
  },
  {
    'file':'String.png',
    'name':'num4_S',
    'x':45,
    'y':52,
    'w':10,
    'h':14
  },
  {
    'file':'String.png',
    'name':'num5_S',
    'x':56,
    'y':52,
    'w':10,
    'h':14
  },
  {
    'file':'String.png',
    'name':'num6_S',
    'x':67,
    'y':52,
    'w':10,
    'h':14
  },
  {
    'file':'String.png',
    'name':'num7_S',
    'x':78,
    'y':52,
    'w':10,
    'h':14
  },
  {
    'file':'String.png',
    'name':'num8_S',
    'x':89,
    'y':52,
    'w':10,
    'h':14
  },
  {
    'file':'String.png',
    'name':'num9_S',
    'x':100,
    'y':52,
    'w':10,
    'h':14
  },
  {
    'file':'String.png',
    'name':'minus_S',
    'x':111,
    'y':52,
    'w':10,
    'h':14
  },
  {
    'file':'String.png',
    'name':'numBlack_S',
    'x':122,
    'y':52,
    'w':10,
    'h':14
  },
  {
    'file':'String.png',
    'name':'dot_S',
    'x':14,
    'y':86,
    'w':4,
    'h':14
  },
  { # アイコン
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
    'name':'IconPress',
    'x':5,
    'y':125,
    'w':50,
    'h':50
  },
  {
    'file':'main1.png',
    'name':'IconCo2',
    'x':8,
    'y':193,
    'w':43,
    'h':35
  },
  { # 単位
    'file':'main1.png',
    'name':'UnitTemp',
    'x':188,
    'y':23,
    'w':26,
    'h':20
  },
  {
    'file':'main1.png',
    'name':'UnitHumi',
    'x':188,
    'y':83,
    'w':23,
    'h':20
  },
  {
    'file':'main1.png',
    'name':'UnitPress',
    'x':188,
    'y':143,
    'w':47,
    'h':20
  },
  {
    'file':'main1.png',
    'name':'UnitCo2',
    'x':188,
    'y':206,
    'w':47,
    'h':17
  },
  { # 統計文字
    'file':'Log_Temp.png',
    'name':'Max',
    'x':61,
    'y':6,
    'w':29,
    'h':12
  },
  {
    'file':'Log_Temp.png',
    'name':'Ave',
    'x':61,
    'y':24,
    'w':27,
    'h':12
  },
  {
    'file':'Log_Temp.png',
    'name':'Min',
    'x':61,
    'y':42,
    'w':28,
    'h':12
  },
  { # 統計数値下線
    'file':'Log_Temp.png',
    'name':'Max_ShortLine',
    'x':60,
    'y':19,
    'w':92,
    'h':1
  },
  {
    'file':'Log_Temp.png',
    'name':'Ave_ShortLine',
    'x':60,
    'y':37,
    'w':92,
    'h':1
  },
  {
    'file':'Log_Temp.png',
    'name':'Min_ShortLine',
    'x':60,
    'y':55,
    'w':92,
    'h':1
  },
  { # 時計コロン
    'file':'Log_Temp.png',
    'name':'Colon',
    'x':203,
    'y':6,
    'w':5,
    'h':12
  },
  { # グラフタイトル
    'file':'Log_Temp.png',
    'name':'Minute',
    'x':3,
    'y':63,
    'w':54,
    'h':12
  },
  {
    'file':'Log_Temp.png',
    'name':'Hour',
    'x':3,
    'y':152,
    'w':37,
    'h':12
  },
  { # グラフ
    'file':'Log_Humi.png',
    'name':'GraphMinuteBack',
    'x':0,
    'y':76,
    'w':12,
    'h':57
  },
  {
    'file':'Log_Humi.png',
    'name':'GraphHourBack',
    'x':0,
    'y':166,
    'w':10,
    'h':57
  },
  {
    'file':'Log_Temp.png',
    'name':'GraphMinuteAverageLine',
    'x':0,
    'y':80,
    'w':12,
    'h':1
  },
  {
    'file':'Log_Temp.png',
    'name':'GraphHourAverageLine',
    'x':0,
    'y':180,
    'w':10,
    'h':1
  },
  {
    'file':'Log_Temp.png',
    'name':'plot_M_0',
    'x':232,
    'y':100,
    'w':4,
    'h':3
  },
  {
    'file':'Log_Temp.png',
    'name':'plot_M_1',
    'x':220,
    'y':100,
    'w':4,
    'h':3
  },
  {
    'file':'Log_Temp.png',
    'name':'plot_M_2',
    'x':208,
    'y':100,
    'w':4,
    'h':3
  },
  {
    'file':'Log_Temp.png',
    'name':'plot_M_3',
    'x':196,
    'y':100,
    'w':4,
    'h':3
  },
  {
    'file':'Log_Temp.png',
    'name':'plot_M_4',
    'x':184,
    'y':100,
    'w':4,
    'h':3
  },
  {
    'file':'Log_Temp.png',
    'name':'plot_M_5',
    'x':172,
    'y':100,
    'w':4,
    'h':3
  },
  {
    'file':'Log_Temp.png',
    'name':'plot_M_6',
    'x':160,
    'y':100,
    'w':4,
    'h':3
  },
  {
    'file':'Log_Temp.png',
    'name':'plot_M_7',
    'x':148,
    'y':100,
    'w':4,
    'h':3
  },
  {
    'file':'Log_Temp.png',
    'name':'plot_M_8',
    'x':136,
    'y':100,
    'w':4,
    'h':3
  },
  {
    'file':'Log_Temp.png',
    'name':'plot_M_9',
    'x':124,
    'y':100,
    'w':4,
    'h':3
  },
  {
    'file':'Log_Temp.png',
    'name':'plot_M_10',
    'x':112,
    'y':100,
    'w':4,
    'h':3
  },
  {
    'file':'Log_Temp.png',
    'name':'plot_M_11',
    'x':100,
    'y':100,
    'w':4,
    'h':3
  },
  {
    'file':'Log_Temp.png',
    'name':'plot_M_12',
    'x':88,
    'y':100,
    'w':4,
    'h':3
  },
  {
    'file':'Log_Temp.png',
    'name':'plot_M_13',
    'x':76,
    'y':100,
    'w':4,
    'h':3
  },
  {
    'file':'Log_Temp.png',
    'name':'plot_M_14',
    'x':64,
    'y':100,
    'w':4,
    'h':3
  },
  {
    'file':'Log_Temp.png',
    'name':'plot_M_15',
    'x':52,
    'y':100,
    'w':4,
    'h':3
  },
  {
    'file':'Log_Temp.png',
    'name':'plot_M_16',
    'x':40,
    'y':100,
    'w':4,
    'h':3
  },
  {
    'file':'Log_Temp.png',
    'name':'plot_M_17',
    'x':28,
    'y':100,
    'w':4,
    'h':3
  },
  {
    'file':'Log_Temp.png',
    'name':'plot_M_18',
    'x':16,
    'y':100,
    'w':4,
    'h':3
  },
  {
    'file':'Log_Temp.png',
    'name':'plot_M_19',
    'x':4,
    'y':100,
    'w':4,
    'h':3
  },
  {
    'file':'Log_Temp.png',
    'name':'plot_H_0',
    'x':232,
    'y':200,
    'w':6,
    'h':3
  },
  {
    'file':'Log_Temp.png',
    'name':'plot_H_1',
    'x':222,
    'y':200,
    'w':6,
    'h':3
  },
  {
    'file':'Log_Temp.png',
    'name':'plot_H_2',
    'x':212,
    'y':200,
    'w':6,
    'h':3
  },
  {
    'file':'Log_Temp.png',
    'name':'plot_H_3',
    'x':202,
    'y':200,
    'w':6,
    'h':3
  },
  {
    'file':'Log_Temp.png',
    'name':'plot_H_4',
    'x':192,
    'y':200,
    'w':6,
    'h':3
  },
  {
    'file':'Log_Temp.png',
    'name':'plot_H_5',
    'x':182,
    'y':200,
    'w':6,
    'h':3
  },
  {
    'file':'Log_Temp.png',
    'name':'plot_H_6',
    'x':172,
    'y':200,
    'w':6,
    'h':3
  },
  {
    'file':'Log_Temp.png',
    'name':'plot_H_7',
    'x':162,
    'y':200,
    'w':6,
    'h':3
  },
  {
    'file':'Log_Temp.png',
    'name':'plot_H_8',
    'x':152,
    'y':200,
    'w':6,
    'h':3
  },
  {
    'file':'Log_Temp.png',
    'name':'plot_H_9',
    'x':142,
    'y':200,
    'w':6,
    'h':3
  },
  {
    'file':'Log_Temp.png',
    'name':'plot_H_10',
    'x':132,
    'y':200,
    'w':6,
    'h':3
  },
  {
    'file':'Log_Temp.png',
    'name':'plot_H_11',
    'x':122,
    'y':200,
    'w':6,
    'h':3
  },
  {
    'file':'Log_Temp.png',
    'name':'plot_H_12',
    'x':112,
    'y':200,
    'w':6,
    'h':3
  },
  {
    'file':'Log_Temp.png',
    'name':'plot_H_13',
    'x':102,
    'y':200,
    'w':6,
    'h':3
  },
  {
    'file':'Log_Temp.png',
    'name':'plot_H_14',
    'x':92,
    'y':200,
    'w':6,
    'h':3
  },
  {
    'file':'Log_Temp.png',
    'name':'plot_H_15',
    'x':82,
    'y':200,
    'w':6,
    'h':3
  },
  {
    'file':'Log_Temp.png',
    'name':'plot_H_16',
    'x':72,
    'y':200,
    'w':6,
    'h':3
  },
  {
    'file':'Log_Temp.png',
    'name':'plot_H_17',
    'x':62,
    'y':200,
    'w':6,
    'h':3
  },
  {
    'file':'Log_Temp.png',
    'name':'plot_H_18',
    'x':52,
    'y':200,
    'w':6,
    'h':3
  },
  {
    'file':'Log_Temp.png',
    'name':'plot_H_19',
    'x':42,
    'y':200,
    'w':6,
    'h':3
  },
  {
    'file':'Log_Temp.png',
    'name':'plot_H_20',
    'x':32,
    'y':200,
    'w':6,
    'h':3
  },
  {
    'file':'Log_Temp.png',
    'name':'plot_H_21',
    'x':22,
    'y':200,
    'w':6,
    'h':3
  },
  {
    'file':'Log_Temp.png',
    'name':'plot_H_22',
    'x':12,
    'y':200,
    'w':6,
    'h':3
  },
  {
    'file':'Log_Temp.png',
    'name':'plot_H_23',
    'x':2,
    'y':200,
    'w':6,
    'h':3
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

def getColorLevel(level):
  r = int(level / 8) & 0x1F # 赤は5bit
  g = int(level / 4) & 0x3F # 緑は6bit
  b = int(level / 8) & 0x1F # 青は5bit
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
        # 違う色に変化
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
    hfile.write('#endif /* RESOURCE */\n')
    hfile.write('\n')
    hfile.write('#endif /* RESOURCE_H_ */\n')
    hfile.write('\n')
  

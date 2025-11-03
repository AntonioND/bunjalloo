#!/usr/bin/env python

files="""
arrow_left
arrow_right
stop
arrow_refresh
--
*arrow_left
*arrow_right
wrench
hourglass
--
book
world_go
find
disk
--
transmit_blue
transmit
transmit_error
attach
--
book_add
cookie
cookie+add
find+add
    """

row_imgs = []
tmp_imgs = []

def exe(cmd):
  import os
  os.system(cmd)

def make_tmp():
  import tempfile
  tmp = tempfile.mktemp(suffix='.png')
  tmp_imgs.append(tmp)
  return tmp

path_to_imgs = '~/Downloads/silk/'

def convert_img(img_name):
  if (img_name.startswith('*')):
    # make it b+w
    real_name = path_to_imgs+'/%s.png'%(img_name[1:])
    tmp = make_tmp()
    cmd = 'convert -channel Red -separate %s %s'%( real_name, tmp )
    exe(cmd)
    return tmp
  elif (img_name.find('+') != -1):
    # overlay images
    real_name = path_to_imgs+'/%s.png'%(img_name.split('+')[0])
    composite = path_to_imgs+'/%s.png'%(img_name.split('+')[1])
    # make composite smaller..
    small = make_tmp()
    cmd = 'convert %s -resize 50%% %s'%(composite, small)
    exe(cmd)
    tmp = make_tmp()
    cmd = 'composite -gravity SouthEast %s %s %s'%(small, real_name, tmp )
    exe(cmd)
    return tmp
  else:
    real_name = path_to_imgs+'/%s.png'%(img_name)
    return real_name

for row in files.split('--'):
  imgs = row.split()
  imgs_for_row = []
  cmd = 'convert +append '
  for i in imgs:
    cmd += convert_img(i)
    cmd += ' '
  tmp = make_tmp()
  cmd += tmp
  exe(cmd)
  row_imgs.append(tmp)

cmd = 'convert -append '+' '.join(row_imgs)
cmd += ' -depth 8 -type Palette outfile.png '
exe(cmd)

import os
for i in tmp_imgs:
  os.unlink(i)

# convert -crop 16x16+16+64 toolbar.png cookie.png
# this slices out a chunk of the toolbar.png


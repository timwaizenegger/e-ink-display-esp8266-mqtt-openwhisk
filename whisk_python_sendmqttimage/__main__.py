#!/usr/bin/env python3
# -*- coding: utf-8 -*-



"""
Tim Waizenegger (c) 2017, 2019

Used as an open whisk action on IBM cloud

follow guide from
https://console.bluemix.net/docs/openwhisk/openwhisk_actions.html#creating-and-invoking-javascript-actions

upload using
rm -rf virtualenv
sudo docker run --rm -v "$PWD:/tmp" openwhisk/python3action sh -c "cd tmp; virtualenv virtualenv; source virtualenv/bin/activate; pip install -r requirements.txt;"
zip -r p.zip .

bx wsk action update send_to_display --main main --kind python:3 p.zip
bx wsk action create send_to_display_white --kind python:3 --main full_white p.zip
bx wsk action create send_to_display_test --kind python:3 --main big_text p.zip
bx wsk action create send_to_display_base64 --kind python:3 --main from_base64 p.zip

"""

import paho.mqtt.client as mqtt
import sys
from PIL import ImageFont, ImageDraw
from PIL import Image
from io import BytesIO
import base64
import re

mq_id = "ibmcloud"
mq_authtoken = "xxx"
mq_serverUrl = "timsrv.de"


def connect_and_send(data):
	client = mqtt.Client(client_id=mq_id, clean_session=True, userdata=None, protocol=mqtt.MQTTv311)
	client.username_pw_set(username=mq_id, password=mq_authtoken)
	client.connect(mq_serverUrl, 1883, 60)
	client.publish(topic="eink2/byte-image-data", payload=data, qos=1, retain=False)
	client.loop()

def main(args):
	connect_and_send(data="aaabbbbbbbbbcccccccccc")
	return { 'message': 'data sent' }

def full_white(args):
	connect_and_send(data="3"*15000)
	return { 'message': 'data sent' }

def big_text(args):
	im = Image.new("1", (400,300))
	draw = ImageDraw.Draw(im)
	#fnt = ImageFont.truetype('Pillow/Tests/fonts/FreeMono.ttf', 40)
	#font = ImageFont.truetype("HelveticaRoundedLTStd-Black.otf", 75)
	draw.text((10, 10), "hello", fill=1) #, font=font)
	draw.arc([(100, 100), (200, 200)], 0, 300, fill=1)
	
	connect_and_send(data=im.tobytes())
	return { 'message': 'data sent' }

def from_base64(args):
	base64_data = args["base64_data"]
	base64_data = re.sub('^data:image/.+;base64,', '', base64_data)
	im = Image.open(BytesIO(base64.b64decode(base64_data)))
	im_pixels = im.convert("1")
	connect_and_send(data=im_pixels.tobytes())
	return { 'message': 'data sent' }

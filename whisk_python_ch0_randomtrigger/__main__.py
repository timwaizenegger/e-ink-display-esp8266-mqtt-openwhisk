#!/usr/bin/env python3
# -*- coding: utf-8 -*-



"""
Tim Waizenegger (c) 2018,2019

Used as an open whisk action on IBM cloud

bx wsk action update displaychannel_ch0_trigger_random --kind python:3 --main main __main__.py
"""

import random
import requests
  
functions_apikey = "cloud functions CF API key"


endpoints_protocol = "https://"
endpoints = [
  "eu-de.functions.cloud.ibm.com/api/v1/namespaces/timw_dev/triggers/displaychannel_ch0_trigger_time",
  "eu-de.functions.cloud.ibm.com/api/v1/namespaces/timw_dev/triggers/displaychannel_ch0_trigger_bitcoin",
  "eu-de.functions.cloud.ibm.com/api/v1/namespaces/timw_dev/triggers/displaychannel_ch0_trigger_twitter",
  "eu-de.functions.cloud.ibm.com/api/v1/namespaces/timw_dev/triggers/displaychannel_ch0_trigger_christmas"
  ]

def main(args):
  endpoint = random.choice(endpoints)
  url = endpoints_protocol + functions_apikey + "@" + endpoint
  ret=requests.post(url)
  print("Made API call to ", url, " , RC is: ", ret)
  return { 'message': 'main method called' }


#main(None)

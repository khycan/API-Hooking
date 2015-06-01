import urllib.request as ur
import re

html = ur.urlopen("http://www.pythonchallenge.com/pc/def/ocr.html")

source = html.read()

source = source.decode('utf-8')

source = source[source.find("<!--\n%%$"):]

source = re.sub("[^a-zA-Z0-9]","",source)

print (source)

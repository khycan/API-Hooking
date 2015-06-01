import urllib.request as ur
import re

html = ur.urlopen("http://www.pythonchallenge.com/pc/def/equality.html")

source = html.read()

source = source.decode('utf-8')

source = source[source.find("<!--\nkAewtlo"):]

print(source.find("body"))

source = source[source.find("EXACTLY"):]

##source = re.sub("[^EXACTLY]","",source)

print (source)

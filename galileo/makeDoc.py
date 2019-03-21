import os, glob

PATH = "./outputs/"
SCRIPT = PATH + "script.txt"
FORM = "./resource/form"
NEXTLINE = "./resource/nextLine"
USER = PATH + "user.txt"
HSBDOC = PATH + "hsb.doc"

AGENDA_PATH = PATH + "Agenda.xml"
KEYWORD_PATH = PATH + "keyword.xml"

#file open
scriptFile = open(SCRIPT,'r')
nextLineFile = open(NEXTLINE,'r')
formFile = open(FORM,'r')
userFile = open(USER,'rw')
HSB_docFile = open(HSBDOC,'w')

#file read
formRead = formFile.read()
nextLineStr = nextLineFile.read()
userArr = userFile.readlines() #user name
userHalf = (len(userArr) / 2) + (len(userArr) % 2)
scriptRead = scriptFile.readlines()

scriptWord = []
for line in scriptRead:
	words = line.split(' ')
	for word in words:
		scriptWord.append(word)

#extract keyword
keyWord = []
HSBname = scriptWord[scriptWord.index('[Name]')+1]
HSBname = HSBname[:len(HSBname)-1]
date = scriptWord[scriptWord.index('[Time]')+1]
startTime = scriptWord[scriptWord.index('[Time]')+2]
endTime = scriptWord[scriptWord.index('[Time]')+5]
endTime = endTime[:len(endTime)-1]

# make agenda string
agenda = ""
numAgen = 1
agenFile = open(AGENDA_PATH, 'r')
if agenFile != None:
	agenCont = agenFile.read()
	agenIdx = agenCont.find("<Text>")
	while agenIdx > 0:
		agenEndIdx = agenCont.find("</Text>")
		agen = agenCont[agenIdx+len("<Text>"):agenEndIdx]
		if agen != "***":
			if numAgen > 1:
				agenda += nextLineStr
			agenda += str(numAgen)
			agenda += ". "
			agenda += agen
		agenCont = agenCont[agenEndIdx+len("</Text>"):]
		agenIdx = agenCont.find("<Text>")

agenFile.close()


#insert keyword
formTok = formRead.split('#')
HSB_docFile.write(formTok[0])

for i in range(0, len(userArr)):
	userArr[i] = userArr[i][:len(userArr[i])-1]

# user
numUser = len(userArr)
if numUser % 2 == 1: # numUser is odd number
	tmp = len(userArr) / 2 + 1
else:
	tmp = len(userArr) / 2
	
user1 = ""
for i in range(0, tmp):
	user1 += (userArr[i])
	user1 += nextLineStr
	user1 += nextLineStr
	user1 += nextLineStr
user2 = ""
for i in range(tmp, numUser):
	user2 += (userArr[i])
	user2 += nextLineStr
	user2 += nextLineStr
	user2 += nextLineStr

for i in range(1, len(formTok)):
	if formTok[i].startswith("0"):
		HSB_docFile.write(HSBname)
	elif formTok[i].startswith("1"):
		HSB_docFile.write(date)
	elif formTok[i].startswith("2"):
		HSB_docFile.write(startTime)
	elif formTok[i].startswith("3"):
		HSB_docFile.write(endTime)
	elif formTok[i].startswith("4"):
		HSB_docFile.write(user1)
	elif formTok[i].startswith("5"):
		HSB_docFile.write(user2)
	elif formTok[i].startswith("6"):
		HSB_docFile.write(agenda)
	elif formTok[i].startswith("7"):
		for line in scriptRead:
			if line.startswith("[Keyword]"): 
				break
			HSB_docFile.write(line)
			HSB_docFile.write(nextLineStr)
			
	HSB_docFile.write(formTok[i][1:])

#file close
scriptFile.close()
nextLineFile.close()
formFile.close()
userFile.close()
HSB_docFile.close()


# save history
filename = PATH
dateTok = date.split('.')
filename = filename + dateTok[0] + dateTok[1] + dateTok[2]
timeTok = startTime.split(':')

#debug
if timeTok[2][len(timeTok[2])-1] == '\n':
	timeTok[2] = timeTok[2][:len(timeTok)-1]
filename = filename + timeTok[0] + timeTok[1] + timeTok[2]

os.rename(HSBDOC, filename + ".doc")

# append info to history/info.xml 
INFO_PATH = "./history/info.xml"
infoFile = open(INFO_PATH, 'r')
infoLines = infoFile.readlines()
infoFile.close()

infoLines.remove("</TABLE>\n")
infoFile = open(INFO_PATH, 'w')
for line in infoLines:
	infoFile.write(line)


infoFile.write("\t<MEETING>\n");
infoFile.write("\t\t<TITLE>" + HSBname + "</TITLE>\n");
startTime = startTime[:len(startTime)-1]
infoFile.write("\t\t<DATE>" + dateTok[0] + dateTok[1] + dateTok[2] + timeTok[0] + timeTok[1] + timeTok[2] + "</DATE>\n");
infoFile.write("\t</MEETING>\n");
infoFile.write("</TABLE>\n");


# xml-lize script
scFile = open(SCRIPT, 'r')
scLines = scFile.readlines()
scFile.close()

scFile = open(SCRIPT, 'w')
scFile.write('<?xml version="1.0" encoding="UTF-8"?>\n')
scFile.write('<TABLE>\n')

newLine = "&lt;br/&gt;"
flag = False

isAgenda = False
isConclu = False


for line in scLines:
	if line.startswith("[Agenda]"):
		isAgenda = True
		scFile.write('\t<SCRIPT>\n')
		line = line[len("[AGENDA] "):len(line)-1] # remove newLine character
		scFile.write('\t\t<AGENDA>' + line + '</AGENDA>\n')
		scFile.write('\t\t<TEXT>')
		flag = True
	elif line.startswith("[Conclusion]"):
		isConclu = True
		scFile.write('</TEXT>\n')
		line = line[len("[CONCLUSION] "):len(line)-1] # remove newLine character
		scFile.write('\t\t<CONCLUSION>' + line + '</CONCLUSION>\n')
		scFile.write('\t</SCRIPT>\n')
		flag = False
	elif flag:
		line = line[:len(line)-1] # remove newLine character
		if line != "":
			scFile.write(line + newLine + '\n')

if isAgenda == False:
	scFile.write('\t<SCRIPT>\n')
	line = line[len("[AGENDA] "):len(line)-1] # remove newLine character
	scFile.write('\t\t<AGENDA>' + line + '</AGENDA>\n')
	scFile.write('\t\t<TEXT>')

for line in scLines:
	line = line[:len(line)-1] # remove newLine character
	if line != "":
		scFile.write(line + newLine + '\n')

if isConclu == False:
	scFile.write('</TEXT>\n')
	line = line[len("[CONCLUSION] "):len(line)-1] # remove newLine character
	scFile.write('\t\t<CONCLUSION>' + line + '</CONCLUSION>\n')
	scFile.write('\t</SCRIPT>\n')



scFile.write('</TABLE>\n')


os.rename(SCRIPT, filename + "_script.xml")
os.rename(KEYWORD_PATH, filename + "_keyword.xml")
keyHitFiles = glob.glob(PATH + "keyHit*")
for file in keyHitFiles:
	file = file[file.rfind("/")+1:]
	newName = filename + "_" + file
	tfile = PATH + file
	os.rename(tfile, newName)

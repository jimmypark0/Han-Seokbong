while true; do

#init
rm ./inputs/* 
rm ./outputs/* ./outputs/.xml

#server start
node server.js
./hsb.exe
echo "********************** success hsb ********************"

#KLT
iconv -f utf-8 -t euc-kr ./outputs/script.txt > ./outputs/scriptEUC.txt
cd ./KLT/EXE
./index ../../outputs/scriptEUC.txt ../../outputs/resultEUC.txt
cd ../..
iconv -f euc-kr -t utf-8 ./outputs/resultEUC.txt > ./outputs/result.txt


./hsb_analyze.exe
echo "********************** success hsb_analyze ********************"

python makeDoc.py

cp outputs/*_keyword.xml history
cp outputs/*_script.xml history
rm outputs/*.xml outputs/result* outputs/*EUC* outputs/user*
cp outputs/* history

ls history
echo "================================================"

done


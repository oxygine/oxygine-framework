set options=--recursive --style=allman --indent-switches --indent-namespaces --indent-preproc-define --pad-oper --pad-header --unpad-paren --align-pointer=type --convert-tabs  --formatted --lineend=windows --suffix=none --keep-one-line-blocks --keep-one-line-statements

d:\AStyle\bin\AStyle.exe d:\oxygine-framework\oxygine\src\*.cpp d:\oxygine-framework\oxygine\src\*.h %options% --exclude=winnie_alloc --exclude=pugixml --exclude=minizip --exclude=closure --exclude=Tween.cpp
d:\AStyle\bin\AStyle.exe d:\oxygine-sound\src\*.cpp d:\oxygine-sound\src\*.h %options%
d:\AStyle\bin\AStyle.exe d:\oxygine-magicparticles\src\*.cpp d:\oxygine-magicparticles\src\*.h %options%

d:\AStyle\bin\AStyle.exe d:\oxygine-framework\examples\*.cpp d:\oxygine-framework\examples\*.h %options%  --exclude=box2d\box2d
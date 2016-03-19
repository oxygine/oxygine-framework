set options=--recursive --style=allman --indent-switches --indent-namespaces --indent-preproc-define --pad-oper --pad-header --unpad-paren --align-pointer=type --convert-tabs  --formatted --lineend=windows --suffix=none --keep-one-line-blocks --keep-one-line-statements

set astyle=d:\dropbox\tools\AStyle.exe
%astyle% d:\oxygine-framework\oxygine\src\*.cpp d:\oxygine-framework\oxygine\src\*.h %options% --exclude=winnie_alloc --exclude=pugixml --exclude=minizip --exclude=closure --exclude=Tween.cpp

%astyle% d:\oxygine-sound\src\*.cpp d:\oxygine-sound\src\*.h d:\oxygine-sound\examples\*.cpp d:\oxygine-sound\examples\*.h %options%
%astyle% d:\oxygine-flow\src\*.cpp d:\oxygine-flow\src\*.h d:\oxygine-flow\examples\*.cpp d:\oxygine-flow\examples\*.h %options%

%astyle% d:\oxygine-magicparticles\src\*.cpp d:\oxygine-magicparticles\src\*.h %options%

%astyle% d:\oxygine-framework\examples\*.cpp d:\oxygine-framework\examples\*.h %options%  --exclude=box2d\box2d

%astyle% d:\oxygine-movie\examples\*.cpp d:\oxygine-movie\examples\*.h %options%
%astyle% d:\oxygine-movie\src\*.cpp d:\oxygine-movie\src\*.h %options%


%astyle% d:\oxygine-billing\examples\*.cpp d:\oxygine-billing\examples\*.h %options%
%astyle% d:\oxygine-billing\src\*.cpp d:\oxygine-billing\src\*.h %options%

%astyle% d:\oxygine-facebook\example\*.cpp d:\oxygine-facebook\example\*.h %options%
%astyle% d:\oxygine-facebook\src\*.cpp d:\oxygine-facebook\src\*.h %options%

%astyle% d:\oxygine-editor\editor\src\*.cpp d:\oxygine-editor\editor\src\*.h %options%
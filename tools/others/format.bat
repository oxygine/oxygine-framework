set options=--recursive --style=allman --indent-switches --indent-namespaces --indent-preproc-define --pad-oper --pad-header --unpad-paren --align-pointer=type --convert-tabs  --formatted --lineend=windows --suffix=none --keep-one-line-blocks --keep-one-line-statements

set astyle=d:\dropbox\tools\AStyle.exe
set root=%CD%\..\..\..\

echo %root%
%astyle% %root%\oxygine-framework\oxygine\src\oxygine\*.cpp %root%\oxygine-framework\oxygine\src\oxygine\*.h %options% --exclude=winnie_alloc --exclude=pugixml --exclude=minizip --exclude=closure --exclude=Tween.cpp

%astyle% %root%\oxygine-sound\src\*.cpp %root%\oxygine-sound\src\*.h %root%\oxygine-sound\examples\*.cpp %root%\oxygine-sound\examples\*.h %options%
%astyle% %root%\oxygine-flow\src\*.cpp %root%\oxygine-flow\src\*.h %root%\oxygine-flow\examples\*.cpp %root%\oxygine-flow\examples\*.h %options%

%astyle% %root%\oxygine-magicparticles\src\*.cpp %root%\oxygine-magicparticles\src\*.h %options%

%astyle% %root%\oxygine-framework\examples\*.cpp %root%\oxygine-framework\examples\*.h %options%  --exclude=box2d\box2d

%astyle% %root%\oxygine-movie\examples\*.cpp %root%\oxygine-movie\examples\*.h %options%
%astyle% %root%\oxygine-movie\src\*.cpp %root%\oxygine-movie\src\*.h %options%


%astyle% %root%\oxygine-billing\examples\*.cpp %root%\oxygine-billing\examples\*.h %options%
%astyle% %root%\oxygine-billing\src\*.cpp %root%\oxygine-billing\src\*.h %options%

%astyle% %root%\oxygine-freetype\examples\*.cpp %root%\oxygine-freetype\examples\*.h %options%
%astyle% %root%\oxygine-freetype\src\*.cpp %root%\oxygine-freetype\src\*.h %options%

%astyle% %root%\oxygine-facebook\example\*.cpp %root%\oxygine-facebook\example\*.h %options%
%astyle% %root%\oxygine-facebook\src\*.cpp %root%\oxygine-facebook\src\*.h %options%

%astyle% %root%\oxygine-editor\editor\src\*.cpp %root%\oxygine-editor\editor\src\*.h %options%
%astyle% %root%\oxygine-editor\examples\*.cpp %root%\oxygine-editor\examples\*.h %options%
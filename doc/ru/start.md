#Начало работы

###Для пользователей Windows и MacOSX
[Скачайте](http://oxygine.org/download.php) Oxygine. Рекомендуется скачать версию **All-In-One** (всё в одном) с SDL. Она включает в себя всё для быстрого и легкого старта и имеет предкомпилированные файлы библиотеки SDL.

Перейдите в папку **oxygine-framework/examples** и откройте любой пример.

Примеры:

* **HelloWorld** - отправная точка.
* **Game** обучающий пример. Он состоит из 4 частей от начинающего до продвинутого пользователя.
* **Demo** включает тесты и демонстрирует функционал Oxygine.
* **DemoBox2D** это пример встроенного физического движка Box2D.
* **Match3** пример игры на Oxygine, где требуется собрать три одинаковых элемента вместе.
 
Каждый пример имеет конфигурационные директории для различных плафторм/ОС

* **proj.win32** - решение для VisualStudio
* **proj.ios** - проект для iOS xcode
* **proj.macosx** - проект для MacOSX xcode
* **proj.marmalade** - проект Marmalade
* *proj.android* - для сборки под Android SDK/NDK
* *proj.cmake* - конфигурационная директория CMake
* *proj.emscripten* - конфигурационная директория Emscripten

> Если у вас возникли какие-либо трудности со сборкой под Android, CMake или Emscripten, посмотрите директорию **oxygine-framework/readme**

###Продвинутым пользователям

Если вы хотите установить и собрать всё вручную, тогда [скачайте](http://oxygine.org/download.php) только oxygine framework.
   
Перейдите в директорию **oxygine-framework/readme** и следуйте инструкциям в файле **readme.txt**

###Marmalade SDK
Если вы планируете разрабатывать с помощью *MarmaladeSDK*, следуйте инструкциям из *Продвинутым пользователям*.

##Следующие шаги
Oxygine включает в себя некоторые инструменты, которые написаны на Python и вы должны установить его, чтобы использовать дополнительные возможности при работе с Oxygine.

* [Скачайте](https://www.python.org/downloads/) и установите Python версии 2.7.*
* [Скачайте](http://www.pythonware.com/products/pil/) и установите PIL для Python


##Создание своего проекта
Вы можете сгенерировать свой проект вне  директории oxygine-framework с помощью скрипта **gen_template.py**. Он расположен здесь:
  
	oxygine-framework/tools/

Запустите скрипт **gen_template.py** с помощью Python, например:

	python gen_template.py MyProjectName -t win32 -d path/to/MyProject/

После выполнения данной команды, будет создано пустое решение *MyProject* для Visual Studio в папке *path/to/MyProject*

для более подробной информации, прочитайте справку: 
	
	python gen_template.py --help

Game has 5 parts (projects) for learning oxygine.
Each part has some new improvements:
1.
- basic Oxygine features
- Sprite, Actor, Tween, 
- handling touch events

2.
- more solid gameplay and game classes

3.
- added scenes and buttons
- custom MyButton actor
- scenes: GameScene and MainMenuScene
- all gameplay moved to GameScene

4. 
- refactored scenes and added transitions between them
- added new GameMenu
- added custom events
- makefourcc
- class Scene is more complete

5. 
- don't have any c++ code changes
- changed data folder structure and xml file:
  - all images were moved outside of data folder
  - generated atlasses with oxygine tool "oxyresbuild"
- see "gen-atlasses" command line tools for builing atlasses
- has low definition atlasses

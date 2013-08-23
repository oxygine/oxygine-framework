Bitmap Font Generator v1.14 beta by Andreas Jönsson (www.AngelCode.com)
----------------------------------------------------------------------------------------
This program will allow you to generate bitmap fonts from TrueType and other Windows fonts. The application generates both image files and character descriptions that can be read by a game for easy rendering of fonts.

Disclaimer
------------------------------
This program is provided 'as-is' and no responsibility for any type of damage taken to your system as a consequence of installing or using this program is taken by the author.

Freeware
-------------------------------
This program is freeware and may be used freely without charge. If you wish to distribute this program contact the author for his permission.

Support
--------------------------------
This program has very limited support as it is freeware, but you are welcome to send your suggestions and questions to the author.

Contact
--------------------------------
The program and any possible upgrades can be found at www.AngelCode.com. The author may also be contacted through this site.

Changes
-------------------------------
1.14 beta - 2013/06/15
- Fixed crash with large fonts when Windows API incorrectly reported negative width for glyphs.
- Improved handling of out-of-memory conditions.
- Fixed crash that could occur when exporting unicode characters above 0x10000 with kerning pairs.
- Fixed crash when command line argument -c was a relative path and the configuration included font file or image files.
- Fixed crash when saving configuration file with added font file from the same directory.

1.13 - 2012/08/12
- Added option to export fonts using fixed cell heights for easier post processing.
- Improved support for exporting kerning pairs for true types that use the GPOS table instead of KERN table.
- Kerning pairs are no longer exported for char ids that have been overridden with imported icons.
- Improved glyph packing when there are large out of proportion images.
- The final texture no longer has boxes around glyphs when a channel is inverted or set to one.
- Fixed subpixel clipping in supersampled glyphs.
- Implemented an alternative glyph rasterization based on the true type outline.
- The rasterization from true type outline doesn't clip glyphs that extend above or below cell height.
- Allow disabling hinting when rasterizing from true type outline.
- Re-added option for rendering with ClearType with native Windows font engine.
- Fixed bug where saved XML format sometimes didn't close the 'kernings' tag if there were no kerning pairs.
- Fixed bug with duplicate kerning pairs in non-unicode font output.
- Fixed problem where double-clicking in file dialog would sometimes change the selected characters under the mouse.
- Enabled run-length encoding for TGA output.
- Added option to force xoffset and yoffset to 0, and xadvance to width.
- It's now possible to open the app directly with a configuration file from the file explorer.
- When loading a bmfc file the characters are now selected accordingly.
- The font dialog no longer lists rotated fonts, i.e. those prefixed with @.
- Added support for working with a font from a file so it is no longer necessary to install it first.
- Imported images are now stored with a relative path in the configuration file.
- Fixed clipping that could occur on left and right edges when generating a font in italic mode.
- Kerning pairs can now be optionally left out in case they are defunct in the TrueType font.
- The save and load configuration dialogs will now initiate where the last configuration file was loaded or saved.

1.12 - 2009/08/02
- Added presets to the export options to aid choosing the correct values for the texture channels.
- Fixed bug when generating font from command line where the page files in the font descriptor would have the incorrect name.
- Added ability to invert individual texture channels for more export options.
- Added command line tool that will wait for the generation to complete before exiting.
- Fixed the yoffset when the font height was scaled.
- Fixed bug in select chars from file for non-unicode with characters above ASCII 127.
- The number of digits for the page id in the texture files is now adapted to the number of generated pages.
- Added support for adjusting xoffset, yoffset, and advance for imported icons.

1.11b - 2009/03/08
- Fixed a bug in the select chars from file with UTF16 encoded files.
- Fixed a bug in the packing routine that could make characters overlap in rare situations.
- Fixed the selection of unicode ranges by clicking on the check mark in the list.

1.11a - 2008/12/06
- Fixed the subpixel misalignment in the glyph height caused by supersampling.
- Worked around a bug in GDI where the returned glyph height is smaller than the actual glyph bitmap.

1.11 - 2008/10/11
- Fixed a freeze that could happen while selecting characters from file.
- Fixed cropping in some true type fonts.
- Fixed the 'Select All Chars' option in ASCII mode.
- Added option 'Output invalid char glyph'.
- Removed the 'Disable unavailable chars' option.
- Fixed problem where characters not defined in the unicode standard could be selected when selecting chars from file.
- An indicator is now displayed in the character grid where images are imported.
- Fixed bug where main window is moved behind other windows when closing image manager.
- Fixed bug where cancelling the icon image dialog would still effectivate the changes.
- It's now possible to select font size by character height, rather than line height.

1.10b - 2008/06/15
- Fixed the channel indicator in the font descriptor for colored icons.
- Fixed cropping of characters in small fonts.
- Fixed bug with black 8bit textures when the check box for pack chars is checked.

1.10a - 2008/06/08
- Corrected the documentation in regards to the block size value in the binary file format.
- Fixed a bug with the 'Select chars from file' where some character might be skipped between reading blocks of 1kb.
- Fixed a rounding problem with supersampling that made the exported line height be one less than the largest character height.
- Removed the external dependency on the Microsoft DLLs.
- Upgraded libpng to 1.2.29.

1.10 - 2008/05/11
- Selecting characters from file now supports both UTF-8 and UTF-16, with automatic detection via byte-order-mark.
- Fixed a bug where unicode subsets where not shown when disable undefined characters was unselected.
- Status bar shows character id for the character under the mouse cursor.
- Added "clear all" option.
- Preview window is now automatically resized to fit the texture when scaling.
- Added support for unicode characters above 0xFFFF.
- Added support for generating textures with DXT1, DXT3, and DXT5 compression.
- Split the font settings dialog in two dialogs, one for the font graphics and one for export options.
- The content of each texture channel can now be customized.
- File format updated to accomodate new feature.

1.9c - 2007/11/16
- Fixed the reversed spacing values in the font settings dialog.
- The font generation can now be aborted while the characters are being added to the texture.
- Greatly improved the speed of generating textures.

1.9b - 2007/10/28
- Fixed the padding.
- Improved documentation.

1.9a - 2007/09/16
- Allowing unicode for all fonts, even fonts that have only one code page.
- Fixed the status messages when generating fonts.

1.9 - 2007/08/19
- Added support for importing colored icons.
- Supported input image formats are: bmp, tga, png, jpg, dds.
- Added support for giving a black outline to characters.
- Characters with outline may be encoded in 1 channel to save space.
- Outline is generated with rounded corners.
- Increased the binary file version because there is new information.

1.8c - 2007/01/25
- Fixed a bug where the font saver got confused with the 4 chnl packed flag set, even though saving in 8bit format.

1.8b - 2007/01/06
- Fixed kerning amount in binary file format when using super sampling.

1.8a - 2006/11/23
- Fixed alignment bug with 8bit TGA and DDS files with non-power-of-4 widths.
- Fixed crash on Windows with installed Input Method Editors, e.g. Japanese and Chinese Windows.

1.8 - 2006/11/11
- Added support for binary font descriptor file.
- Added support for saving PNG texture files.
- The application now remembers the configuration from last execution.
- Improved useability for selecting characters in unicode mode.
- Added option to select all characters from a text file.
- Kerning pairs with amount 0 are no longer output.
- XML font descriptor file now groups the pages, chars, and kerning pairs.
- The font configuration can be saved to disk, and later reloaded.
- Added support for generating fonts via command line using a saved configuration. 
- Added support for saving DDS texture files (no compression).

1.7a - 2006/09/12
- Fixed the creation of non-unicode fonts.

1.7 - 2006/09/08
- Added the standard xml header.
- Added support for unicode charsets.
- Added the option to pack monochrome characters into each of the 4 channels of a 32 bit texture.
- Added tag with texture file names in the font descriptor file.
- The generation of textures is now done in a background thread.

1.6 - 2006/02/18
- It's now possible to choose between XML and text for the format of the font descriptor file.
- Fixed kerning sizes with super-sampling.

1.5 - 2005/10/01
- Implemented stretching so that fonts can now be non-uniformly scaled (only works on Win2K or later).

1.4a - 2005/08/26
- Non-TrueType fonts, such as Script and System, are now supported as well.
- Minor fix to rendering when padding is used.

1.4 - 2005/07/17
- The preview window now shows the page number in the title.
- Characters without a defined glyph may now be automatically removed (only works on Win2K or later).
- Kerning pairs are now saved as well.
- Added support for chooseable charset, e.g. arabic and hebrew fonts.

1.3 - 2005/05/08
- Added support for italic fonts.
- Application no longer crashes when visualizing with no characters selected.

1.2 - 2005/03/09
- User can now choose to generate the bitmap font with or without font smoothing.
- Font smoothing is now done without ClearType, so that artifacts are no longer generated in the output texture.
- Added program icon.

1.1 - 2005/03/05 
- User can now choose to save in 32bit and 8bit TGAs.
- The spacing between characters can now be manually set.
- Fixed the bug where some fonts weren't visible in the font list.

1.0a - 2004/11/27 
- Fixed a problem with values that where not adjusted to compensate for antialiasing level.

1.0 - 2004/02/14 
- First public version.

copyright (c) 2004-2013 Andreas Jönsson

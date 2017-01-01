#pragma once
#include "oxygine-include.h"
#include "Actor.h"
#include "TextStyle.h"
#include "VisualStyle.h"

namespace oxygine
{
    DECLARE_SMART(TextField, spTextField);
    class TextField : public VStyleActor
    {
        INHERITED(VStyleActor);
    public:
        static void setDefaultFont(ResFont*);
        static ResFont* getDefaultFont();

        DECLARE_COPYCLONE_NEW(TextField);
        TextField();
        ~TextField();

        void serialize(serializedata* data);
        void deserialize(const deserializedata* data);

        /**Returns current text style*/
        const TextStyle&            getStyle() const {return _style;}
        /**Returns text bounds*/
        const Rect&                 getTextRect() const;
        /**Returns current text*/
        const std::string&          getText() const { return _text; }
        const ResFont*              getFont() const;

        int                         getFontSize() const;
        int                         getLinesOffset() const;
        int                         getKerning() const;
        TextStyle::VerticalAlign    getVAlign() const;
        TextStyle::HorizontalAlign  getHAlign() const;
        bool                        getMultiline() const;
        bool                        getBreakLongWords() const;
        text::Symbol*               getSymbolAt(int pos) const;
        const Color&                getOutlineColor() const;
        float                       getOutline() const;
        const Color&                getStyleColor() const;
        float                       getWeight() const;
        float                       getBaselineScale() const;
        unsigned int                getOptions() const;


        bool getBounds(RectF&) const OVERRIDE;

        /**Overwrites TextStyle Vertical align*/
        void setVAlign(TextStyle::VerticalAlign align);
        /**Overwrites TextStyle Horizontal align*/
        void setHAlign(TextStyle::HorizontalAlign align);
        void setAlign(TextStyle::VerticalAlign, TextStyle::HorizontalAlign);
        /**Overwrites TextStyle multiline*/
        void setMultiline(bool multiline);
        /**Overwrites TextStyle breakLongWords*/
        void setBreakLongWords(bool val);
        /**Overwrites TextStyle linesOffset*/
        void setLinesOffset(int offset);
        /**Overwrites TextStyle baselineScale*/
        void setBaselineScale(float scale);
        /**Overwrites TextStyle kerning*/
        void setKerning(int kerning);
        /**Overwrites TextStyle fontSize*/
        void setFontSize(int size);
        /**Overwrites TextStyle color*/
        void setStyleColor(const Color&);
        /**Overwrites TextStyle options*/
        void setOptions(unsigned int opt);

        /**Overwrites TextStyle font.*/
        void setFont(const ResFont* rs);

        /**Overwrites TextStyle outlineColor. works only with SD font*/
        void setOutlineColor(const Color&);
        /**Overwrites TextStyle outline. works only with SD font*/
        void setOutline(float v);
        /**Overwrites TextStyle outline. works only with SD font*/
        void setWeight(float w);

        void setStyle(const TextStyle& st);
        /**Changes text utf-8 string*/
        void setText(const std::string& str);
        /**Changes text widestring*/
        void setText(const std::wstring& str);
        /**Changes text.
        Supports html style tags:  <br/> - break line,  <div color='FFFFFF'/>colored text </div>, <div c='FFFFFF'/>colored text</div>
        example:
        This is <div c='ff0000'/> colored text <div/>supports  <div c='00ff00AA'/>nested tags</div><br/> with broken line.
        */
        void setHtmlText(const std::string& str);
        void setHtmlText(const std::wstring& str);

        bool isOn(const Vector2& localPosition);


        typedef Property<Color, const Color&, TextField, &TextField::getOutlineColor, &TextField::setOutlineColor>  TweenOutlineColor;


    public:

        std::string dump(const dumpOptions& options) const;
        void doRender(RenderState const& parentRenderState);

        text::Node* getRootNode(float scale);
    protected:
        enum
        {
            flag_html = flag_last << 1,
            flag_rebuild = flag_last << 2,
        };
        std::string  _text;
        TextStyle _style;

        text::Node* _root;
        Rect _textRect;
        float _rtscale;
        int _realFontSize;


        void needRebuild();
        void sizeChanged(const Vector2& size);
    };
}

EDITOR_INCLUDE(TextField);
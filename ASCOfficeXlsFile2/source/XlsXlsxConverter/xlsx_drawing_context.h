#pragma once

#include <vector>
#include <iosfwd>
#include <string.h>

#include "xlsx_drawings.h"

#include "ShapeType.h"	

const std::wstring standart_color[56] = {
		L"000000",L"FFFFFF",L"FF0000",L"00FF00",L"0000FF",L"FFFF00",L"FF00FF",L"00FFFF",
		L"800000",L"008000",L"000080",L"808000",L"800080",L"008080",L"C0C0C0",L"808080",
		L"9999FF",L"993366",L"FFFFCC",L"CCFFFF",L"660066",L"FF8080",L"0066CC",L"CCCCFF",
		L"000080",L"FF00FF",L"FFFF00",L"00FFFF",L"800080",L"800000",L"008080",L"0000FF",
		L"00CCFF",L"CCFFFF",L"CCFFCC",L"FFFF99",L"99CCFF",L"FF99CC",L"CC99FF",L"FFCC99",
		L"3366FF",L"33CCCC",L"99CC00",L"FFCC00",L"FF9900",L"FF6600",L"666699",L"969696",
		L"003366",L"339966",L"003300",L"333300",L"993300",L"993366",L"333399",L"333333"	};

namespace oox {

class external_items;
class xlsx_conversion_context;

class xlsx_drawing_context_handle
{
public:
    xlsx_drawing_context_handle(external_items & items);
    ~xlsx_drawing_context_handle();
    
    std::pair<std::wstring, std::wstring> add_drawing_xml(std::wstring const & content, xlsx_drawings_ptr drawings);
    const std::vector<drawing_elm> & content() const;

    friend class xlsx_drawing_context;
private:
    class Impl;
    _CP_PTR(Impl) impl_;
};

struct _color
{
	_color() : index(-1), bScheme(false), nRGB(0){}
	int				nRGB;
	std::wstring	sRGB;
	int				index;
	bool			bScheme;

	void SetRGB(unsigned char nR, unsigned char  nG, unsigned char  nB);

	unsigned char  GetB() { return (unsigned char )(nRGB);}
	unsigned char  GetG() { return (unsigned char )(nRGB>>8);}
	unsigned char  GetR() { return (unsigned char )(nRGB>>16);}

};

struct _rect
{
	_rect() : left(0), top(0), right(0),bottom(0){}

    int   left;
    int   top;
    int   right;
    int   bottom;

};
enum _fill_type
{
	fillNone,
	fillSolid,
	fillPattern,
	fillTexture,
	fillGradient,
	fillGradientOne
};
enum _texture_mode
{
	textureTill,
	textureStretch
};
enum _line_typeDash
{
	lineSolid,
	lineDash,
	lineDot,
	lineDashDot,
	lineDashDotDot
};
	
class _drawing_state;
typedef _CP_PTR(_drawing_state) _drawing_state_ptr;

class _drawing_state
{
public:
	_drawing_state() :	isInternal(false), 
						shape_id(msosptRectangle),  
						flipH(false), flipV(false), 
						text_wrap(2), //none
						bTextBox(false)
	{
		id			= -1;		
		rotation	= 0;
		type_anchor	= 1;
		parent_drawing_states = NULL;
	}

	int						shape_type;

	external_items::Type	type;
	std::wstring			name;
	std::wstring			description;

	int						type_anchor;
	std::wstring			anchor;
	std::wstring			shape;

	int						id;
	MSOSPT					shape_id;
	
	bool					flipV;
	bool					flipH;
	std::wstring			hyperlink;
	int						rotation;

	std::wstring			path;
	_rect					path_rect;

	std::wstring			text_content;	//c форматированием
	int						text_wrap;
	
	struct _wordart
	{
		_wordart() : bEnabled(false), size(0), align(1), bold(false), italic(false), underline(false), vertical(false) {}
		bool			bEnabled;
		std::wstring	text;	
		std::wstring	font;	
		int				size;
		int				align;
		bool			bold;
		bool			italic;
		bool			underline;
		bool			vertical;
	}wordart;
	
	bool				isInternal;
	bool				bTextBox;

	struct _shadow
	{
		_shadow() {is = false;}
		bool			is;
		_color			color;
		int				opacity;
	}shadow;
	
	struct _fill
	{
		_fill() 
		{
			angle = opacity = opacity2 = focus = 0; type = fillSolid; 
			memset(texture_crop, 0, 4 * sizeof(int));
			texture_crop_enabled = false;
		}
		_color			color;
		_color			color2;
		double			opacity;
		double			opacity2;
		_fill_type		type;

		int				focus;
		double			angle;
		
		std::wstring	texture_target;
		int				texture_crop[4];
		bool			texture_crop_enabled;
		_texture_mode	texture_mode;

		std::vector<std::pair<double, _color>> colorsPosition;
	}fill;

	struct _line
	{
		std::wstring	style;
        int             width;
		_line_typeDash	typeDash;
		_fill			fill;
	}line;	
//for group
	std::vector<_drawing_state_ptr>		drawing_states;
	std::vector<_drawing_state_ptr>*	parent_drawing_states;
};
struct _hlink_desc
{
    std::wstring	sId;
    std::wstring	sLink;
	std::wstring	sDisplay;
	bool			bExternal;
};

class xlsx_drawing_context
{
public:
    xlsx_drawing_context(xlsx_conversion_context & Context);
	~xlsx_drawing_context(){}

	xlsx_drawings_ptr get_drawings();
	bool empty();	

	void start_group();
	bool start_drawing(int type);	
		void start_image();
		void start_shape(int type);
		void start_chart();


        void set_id			(int id);
		void set_FlipH		();
		void set_FlipV		();
		void set_shape_id	(int id);

        void set_name		(const std::wstring & str);
        void set_description(const std::wstring & str);
		
        void set_crop_top	(double val);
        void set_crop_bottom(double val);
        void set_crop_left	(double val);
        void set_crop_right	(double val);

        void set_rotation	(double val);

        void set_fill_color		(int nColor, const std::wstring & sColor, bool background = false);
		void set_fill_color		(int index, int type, bool background = false);
 		void set_fill_opacity	(double val, bool background = false);       
		void set_fill_type		(int val);
		void set_fill_angle		(double val);
		void set_fill_texture_mode(int val);
        void set_fill_texture	(const std::wstring & str);
		void add_fill_colors	(double position, const std::wstring & color);
		void add_fill_colors	(double position, int index, int type);		
		void set_fill_focus	(int val);

		void set_line_color	(int nColor, const std::wstring & color);
		void set_line_color	(int index, int type);
        void set_line_type	(int val);
        void set_line_style	(int val);
        void set_line_width (int val);
		void set_line_dash	(int val);

		void set_chart_sheet_anchor(double width, double height);
        void set_anchor		(const std::wstring & str);
		bool is_anchor		();

        void set_properties	(const std::wstring & str);
        void set_hyperlink	(const std::wstring & link, const std::wstring & display, bool is_external);

		void set_path_rect	(_rect & rect);
		void set_path		(const std::wstring & path);

		void set_text		(const std::wstring & text);
		void set_text_wrap	(int val);
		
		void set_wordart_text		(const std::wstring & text);
		void set_wordart_font		(const std::wstring & text);
		void set_wordart_size		(int val);
		void set_wordart_align		(int val);
		void set_wordart_bold		(bool val);
		void set_wordart_italic		(bool val);
		void set_wordart_underline	(bool val);
		void set_wordart_vertical	(bool val);
		
//------------------------------------------------------------------------------		
		void serialize_shape		(_drawing_state_ptr & drawing_state);			
		void serialize_chart		(_drawing_state_ptr & drawing_state, std::wstring rId );	
		void serialize_pic			(_drawing_state_ptr & drawing_state, std::wstring rId );	

		void serialize				(std::wostream & stream, _drawing_state_ptr & drawing_state);
		void serialize_fill			(std::wostream & stream, _drawing_state_ptr & drawing_state);
		void serialize_fill			(std::wostream & stream);
		void serialize				(std::wostream & stream);

	void end_drawing();
	void end_group();
private:
	int							current_level;
    xlsx_conversion_context		& context_;
	
	xlsx_drawing_context_handle	& handle_;
	xlsx_drawings_ptr			xlsx_drawings_;
	int							count_object;
	bool						in_chart_;
	
	std::vector<_hlink_desc>	hlinks_;

	std::vector<_drawing_state_ptr>		drawing_states;
	std::vector<_drawing_state_ptr>*	current_drawing_states;
	
	void end_drawing			(_drawing_state_ptr & drawing_state);
	void reset_fill_pattern		(_drawing_state_ptr & drawing_state);
	
	void serialize_line			(std::wostream & stream, _drawing_state::_line & line);
	void serialize_fill			(std::wostream & stream, _drawing_state::_fill & fill);
	void serialize_gradient_fill(std::wostream & stream, _drawing_state::_fill & fill);
	void serialize_bitmap_fill	(std::wostream & stream, _drawing_state::_fill & fill, std::wstring rId, const std::wstring ns = L"a:");
	void serialize_solid_fill	(std::wostream & stream, const _color &color, double opacity = 0);
	void serialize_none_fill	(std::wostream & stream);
	
	void serialize_line			(std::wostream & stream, _drawing_state_ptr & drawing_state);
	void serialize_xfrm			(std::wostream & stream, _drawing_state_ptr & drawing_state);
	void serialize_text			(std::wostream & stream, _drawing_state_ptr & drawing_state);
	void serialize_color		(std::wostream & stream, const _color &color, double opacity = 0);

	bool ChangeBlack2ColorImage(std::wstring sRgbColor1, std::wstring sRgbColor2, _drawing_state_ptr & drawing_state);
	_color CorrectSysColor(int nColorCode, _drawing_state_ptr & drawing_state);
};

}

/*
Original code by Lee Thomason (www.grinninglizard.com)

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any
damages arising from the use of this software.

Permission is granted to anyone to use this software for any
purpose, including commercial applications, and to alter it and
redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must
not claim that you wrote the original software. If you use this
software in a product, an acknowledgment in the product documentation
would be appreciated but is not required.

2. Altered source versions must be plainly marked as such, and
must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any source
distribution.
*/

#include "tinyxml2.h"

#include <new>		// yes, this one new style header, is in the Android SDK.
#if defined(ANDROID_NDK) || defined(__QNXNTO__)
#   include <stddef.h>
#   include <stdarg.h>
#else
#   include <cstddef>
#   include <cstdarg>
#endif

#if defined(_MSC_VER) && (_MSC_VER >= 1400 ) && (!defined WINCE)
	// Microsoft Visual Studio, version 2005 and higher. Not WinCE.
	/*int _snprintf_s(
	   char *buffer,
	   size_t sizeOfBuffer,
	   size_t count,
	   const char *format [,
		  argument] ...
	);*/
	static inline int TIXML_SNPRINTF( char* buffer, size_t size, const char* format, ... )
	{
		va_list va;
		va_start( va, format );
		int result = vsnprintf_s( buffer, size, _TRUNCATE, format, va );
		va_end( va );
		return result;
	}

	static inline int TIXML_VSNPRINTF( char* buffer, size_t size, const char* format, va_list va )
	{
		int result = vsnprintf_s( buffer, size, _TRUNCATE, format, va );
		return result;
	}

	#define TIXML_VSCPRINTF	_vscprintf
	#define TIXML_SSCANF	sscanf_s
#elif defined _MSC_VER
	// Microsoft Visual Studio 2003 and earlier or WinCE
	#define TIXML_SNPRINTF	_snprintf
	#define TIXML_VSNPRINTF _vsnprintf
	#define TIXML_SSCANF	sscanf
	#if (_MSC_VER < 1400 ) && (!defined WINCE)
		// Microsoft Visual Studio 2003 and not WinCE.
		#define TIXML_VSCPRINTF   _vscprintf // VS2003's C runtime has this, but VC6 C runtime or WinCE SDK doesn't have.
	#else
		// Microsoft Visual Studio 2003 and earlier or WinCE.
		static inline int TIXML_VSCPRINTF( const char* format, va_list va )
		{
			int len = 512;
			for (;;) {
				len = len*2;
				char* str = new char[len]();
				const int required = _vsnprintf(str, len, format, va);
				delete[] str;
				if ( required != -1 ) {
					TIXMLASSERT( required >= 0 );
					len = required;
					break;
				}
			}
			TIXMLASSERT( len >= 0 );
			return len;
		}
	#endif
#else
	// GCC version 3 and higher
	//#warning( "Using sn* functions." )
	#define TIXML_SNPRINTF	snprintf
	#define TIXML_VSNPRINTF	vsnprintf
	static inline int TIXML_VSCPRINTF( const char* format, va_list va )
	{
		int len = vsnprintf( 0, 0, format, va );
		TIXMLASSERT( len >= 0 );
		return len;
	}
	#define TIXML_SSCANF   sscanf
#endif


static const char LINE_FEED				= (char)0x0a;			// all line endings are normalized to LF
static const char LF = LINE_FEED;
static const char CARRIAGE_RETURN		= (char)0x0d;			// CR gets filtered out
static const char CR = CARRIAGE_RETURN;
static const char SINGLE_QUOTE			= '\'';
static const char DOUBLE_QUOTE			= '\"';

// Bunch of unicode info at:
//		http://www.unicode.org/faq/utf_bom.html
//	ef bb bf (Microsoft "lead bytes") - designates UTF-8

static const unsigned char TIXML_UTF_LEAD_0 = 0xefU;
static const unsigned char TIXML_UTF_LEAD_1 = 0xbbU;
static const unsigned char TIXML_UTF_LEAD_2 = 0xbfU;

namespace tinyxml2
{

struct Entity {
    const char* pattern;
    int length;
    char value;
};

static const int NUM_ENTITIES = 5;
static const Entity entities[NUM_ENTITIES] = {
    { "quot", 4,	DOUBLE_QUOTE },
    { "amp", 3,		'&'  },
    { "apos", 4,	SINGLE_QUOTE },
    { "lt",	2, 		'<'	 },
    { "gt",	2,		'>'	 }
};


StrPair::~StrPair()
{
    Reset();
}


void StrPair::TransferTo( StrPair* other )
{
    if ( this == other ) {
        return;
    }
    // This in effect implements the assignment operator by "moving"
    // ownership (as in auto_ptr).

    TIXMLASSERT( other->_flags == 0 );
    TIXMLASSERT( other->_start == 0 );
    TIXMLASSERT( other->_end == 0 );

    other->Reset();

    other->_flags = _flags;
    other->_start = _start;
    other->_end = _end;

    _flags = 0;
    _start = 0;
    _end = 0;
}

void StrPair::Reset()
{
    if ( _flags & NEEDS_DELETE ) {
        delete [] _start;
    }
    _flags = 0;
    _start = 0;
    _end = 0;
}


void StrPair::SetStr( const char* str, int flags )
{
    Reset();
    size_t len = strlen( str );
    TIXMLASSERT( _start == 0 );
    _start = new char[ len+1 ];
    memcpy( _start, str, len+1 );
    _end = _start + len;
    _flags = flags | NEEDS_DELETE;
}


char* StrPair::ParseText( char* p, const char* endTag, int strFlags )
{
    TIXMLASSERT( endTag && *endTag );

    char* start = p;
    char  endChar = *endTag;
    size_t length = strlen( endTag );

    // Inner loop of text parsing.
    while ( *p ) {
        if ( *p == endChar && strncmp( p, endTag, length ) == 0 ) {
            Set( start, p, strFlags );
            return p + length;
        }
        ++p;
    }
    return 0;
}


char* StrPair::ParseName( char* p )
{
    if ( !p || !(*p) ) {
        return 0;
    }
    if ( !XMLUtil::IsNameStartChar( *p ) ) {
        return 0;
    }

    char* const start = p;
    ++p;
    while ( *p && XMLUtil::IsNameChar( *p ) ) {
        ++p;
    }

    Set( start, p, 0 );
    return p;
}


void StrPair::CollapseWhitespace()
{
    // Adjusting _start would cause undefined behavior on delete[]
    TIXMLASSERT( ( _flags & NEEDS_DELETE ) == 0 );
    // Trim leading space.
    _start = XMLUtil::SkipWhiteSpace( _start );

    if ( *_start ) {
        char* p = _start;	// the read pointer
        char* q = _start;	// the write pointer

        while( *p ) {
            if ( XMLUtil::IsWhiteSpace( *p )) {
                p = XMLUtil::SkipWhiteSpace( p );
                if ( *p == 0 ) {
                    break;    // don't write to q; this trims the trailing space.
                }
                *q = ' ';
                ++q;
            }
            *q = *p;
            ++q;
            ++p;
        }
        *q = 0;
    }
}


const char* StrPair::GetStr()
{
    TIXMLASSERT( _start );
    TIXMLASSERT( _end );
    if ( _flags & NEEDS_FLUSH ) {
        *_end = 0;
        _flags ^= NEEDS_FLUSH;

        if ( _flags ) {
            char* p = _start;	// the read pointer
            char* q = _start;	// the write pointer

            while( p < _end ) {
                if ( (_flags & NEEDS_NEWLINE_NORMALIZATION) && *p == CR ) {
                    // CR-LF pair becomes LF
                    // CR alone becomes LF
                    // LF-CR becomes LF
                    if ( *(p+1) == LF ) {
                        p += 2;
                    }
                    else {
                        ++p;
                    }
                    *q++ = LF;
                }
                else if ( (_flags & NEEDS_NEWLINE_NORMALIZATION) && *p == LF ) {
                    if ( *(p+1) == CR ) {
                        p += 2;
                    }
                    else {
                        ++p;
                    }
                    *q++ = LF;
                }
                else if ( (_flags & NEEDS_ENTITY_PROCESSING) && *p == '&' ) {
                    // Entities handled by tinyXML2:
                    // - special entities in the entity table [in/out]
                    // - numeric character reference [in]
                    //   &#20013; or &#x4e2d;

                    if ( *(p+1) == '#' ) {
                        const int buflen = 10;
                        char buf[buflen] = { 0 };
                        int len = 0;
                        char* adjusted = const_cast<char*>( XMLUtil::GetCharacterRef( p, buf, &len ) );
                        if ( adjusted == 0 ) {
                            *q = *p;
                            ++p;
                            ++q;
                        }
                        else {
                            TIXMLASSERT( 0 <= len && len <= buflen );
                            TIXMLASSERT( q + len <= adjusted );
                            p = adjusted;
                            memcpy( q, buf, len );
                            q += len;
                        }
                    }
                    else {
                        bool entityFound = false;
                        for( int i = 0; i < NUM_ENTITIES; ++i ) {
                            const Entity& entity = entities[i];
                            if ( strncmp( p + 1, entity.pattern, entity.length ) == 0
                                    && *( p + entity.length + 1 ) == ';' ) {
                                // Found an entity - convert.
                                *q = entity.value;
                                ++q;
                                p += entity.length + 2;
                                entityFound = true;
                                break;
                            }
                        }
                        if ( !entityFound ) {
                            // fixme: treat as error?
                            ++p;
                            ++q;
                        }
                    }
                }
                else {
                    *q = *p;
                    ++p;
                    ++q;
                }
            }
            *q = 0;
        }
        // The loop below has plenty going on, and this
        // is a less useful mode. Break it out.
        if ( _flags & NEEDS_WHITESPACE_COLLAPSING ) {
            CollapseWhitespace();
        }
        _flags = (_flags & NEEDS_DELETE);
    }
    TIXMLASSERT( _start );
    return _start;
}




// --------- XMLUtil ----------- //

const char* XMLUtil::ReadBOM( const char* p, bool* bom )
{
    TIXMLASSERT( p );
    TIXMLASSERT( bom );
    *bom = false;
    const unsigned char* pu = reinterpret_cast<const unsigned char*>(p);
    // Check for BOM:
    if (    *(pu+0) == TIXML_UTF_LEAD_0
            && *(pu+1) == TIXML_UTF_LEAD_1
            && *(pu+2) == TIXML_UTF_LEAD_2 ) {
        *bom = true;
        p += 3;
    }
    TIXMLASSERT( p );
    return p;
}


void XMLUtil::ConvertUTF32ToUTF8( unsigned long input, char* output, int* length )
{
    const unsigned long BYTE_MASK = 0xBF;
    const unsigned long BYTE_MARK = 0x80;
    const unsigned long FIRST_BYTE_MARK[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };

    if (input < 0x80) {
        *length = 1;
    }
    else if ( input < 0x800 ) {
        *length = 2;
    }
    else if ( input < 0x10000 ) {
        *length = 3;
    }
    else if ( input < 0x200000 ) {
        *length = 4;
    }
    else {
        *length = 0;    // This code won't convert this correctly anyway.
        return;
    }

    output += *length;

    // Scary scary fall throughs.
    switch (*length) {
        case 4:
            --output;
            *output = (char)((input | BYTE_MARK) & BYTE_MASK);
            input >>= 6;
        case 3:
            --output;
            *output = (char)((input | BYTE_MARK) & BYTE_MASK);
            input >>= 6;
        case 2:
            --output;
            *output = (char)((input | BYTE_MARK) & BYTE_MASK);
            input >>= 6;
        case 1:
            --output;
            *output = (char)(input | FIRST_BYTE_MARK[*length]);
            break;
        default:
            TIXMLASSERT( false );
    }
}


const char* XMLUtil::GetCharacterRef( const char* p, char* value, int* length )
{
    // Presume an entity, and pull it out.
    *length = 0;

    if ( *(p+1) == '#' && *(p+2) ) {
        unsigned long ucs = 0;
        TIXMLASSERT( sizeof( ucs ) >= 4 );
        ptrdiff_t delta = 0;
        unsigned mult = 1;
        static const char SEMICOLON = ';';

        if ( *(p+2) == 'x' ) {
            // Hexadecimal.
            const char* q = p+3;
            if ( !(*q) ) {
                return 0;
            }

            q = strchr( q, SEMICOLON );

            if ( !q ) {
                return 0;
            }
            TIXMLASSERT( *q == SEMICOLON );

            delta = q-p;
            --q;

            while ( *q != 'x' ) {
                unsigned int digit = 0;

                if ( *q >= '0' && *q <= '9' ) {
                    digit = *q - '0';
                }
                else if ( *q >= 'a' && *q <= 'f' ) {
                    digit = *q - 'a' + 10;
                }
                else if ( *q >= 'A' && *q <= 'F' ) {
                    digit = *q - 'A' + 10;
                }
                else {
                    return 0;
                }
                TIXMLASSERT( digit >= 0 && digit < 16);
                TIXMLASSERT( digit == 0 || mult <= UINT_MAX / digit );
                const unsigned int digitScaled = mult * digit;
                TIXMLASSERT( ucs <= ULONG_MAX - digitScaled );
                ucs += digitScaled;
                TIXMLASSERT( mult <= UINT_MAX / 16 );
                mult *= 16;
                --q;
            }
        }
        else {
            // Decimal.
            const char* q = p+2;
            if ( !(*q) ) {
                return 0;
            }

            q = strchr( q, SEMICOLON );

            if ( !q ) {
                return 0;
            }
            TIXMLASSERT( *q == SEMICOLON );

            delta = q-p;
            --q;

            while ( *q != '#' ) {
                if ( *q >= '0' && *q <= '9' ) {
                    const unsigned int digit = *q - '0';
                    TIXMLASSERT( digit >= 0 && digit < 10);
                    TIXMLASSERT( digit == 0 || mult <= UINT_MAX / digit );
                    const unsigned int digitScaled = mult * digit;
                    TIXMLASSERT( ucs <= ULONG_MAX - digitScaled );
                    ucs += digitScaled;
                }
                else {
                    return 0;
                }
                TIXMLASSERT( mult <= UINT_MAX / 10 );
                mult *= 10;
                --q;
            }
        }
        // convert the UCS to UTF-8
        ConvertUTF32ToUTF8( ucs, value, length );
        return p + delta + 1;
    }
    return p+1;
}


void XMLUtil::ToStr( int v, char* buffer, int bufferSize )
{
    TIXML_SNPRINTF( buffer, bufferSize, "%d", v );
}


void XMLUtil::ToStr( unsigned v, char* buffer, int bufferSize )
{
    TIXML_SNPRINTF( buffer, bufferSize, "%u", v );
}


void XMLUtil::ToStr( bool v, char* buffer, int bufferSize )
{
    TIXML_SNPRINTF( buffer, bufferSize, "%d", v ? 1 : 0 );
}

/*
	ToStr() of a number is a very tricky topic.
	https://github.com/leethomason/tinyxml2/issues/106
*/
void XMLUtil::ToStr( float v, char* buffer, int bufferSize )
{
    TIXML_SNPRINTF( buffer, bufferSize, "%.8g", v );
}


void XMLUtil::ToStr( double v, char* buffer, int bufferSize )
{
    TIXML_SNPRINTF( buffer, bufferSize, "%.17g", v );
}


bool XMLUtil::ToInt( const char* str, int* value )
{
    if ( TIXML_SSCANF( str, "%d", value ) == 1 ) {
        return true;
    }
    return false;
}

bool XMLUtil::ToUnsigned( const char* str, unsigned *value )
{
    if ( TIXML_SSCANF( str, "%u", value ) == 1 ) {
        return true;
    }
    return false;
}

bool XMLUtil::ToBool( const char* str, bool* value )
{
    int ival = 0;
    if ( ToInt( str, &ival )) {
        *value = (ival==0) ? false : true;
        return true;
    }
    if ( StringEqual( str, "true" ) ) {
        *value = true;
        return true;
    }
    else if ( StringEqual( str, "false" ) ) {
        *value = false;
        return true;
    }
    return false;
}


bool XMLUtil::ToFloat( const char* str, float* value )
{
    if ( TIXML_SSCANF( str, "%f", value ) == 1 ) {
        return true;
    }
    return false;
}

bool XMLUtil::ToDouble( const char* str, double* value )
{
    if ( TIXML_SSCANF( str, "%lf", value ) == 1 ) {
        return true;
    }
    return false;
}


char* XMLDocument::Identify( char* p, XMLNode** node )
{
    TIXMLASSERT( node );
    TIXMLASSERT( p );
    char* const start = p;
    p = XMLUtil::SkipWhiteSpace( p );
    if( !*p ) {
        *node = 0;
        TIXMLASSERT( p );
        return p;
    }

    // These strings define the matching patterns:
    static const char* xmlHeader		= { "<?" };
    static const char* commentHeader	= { "<!--" };
    static const char* cdataHeader		= { "<![CDATA[" };
    static const char* dtdHeader		= { "<!" };
    static const char* elementHeader	= { "<" };	// and a header for everything else; check last.

    static const int xmlHeaderLen		= 2;
    static const int commentHeaderLen	= 4;
    static const int cdataHeaderLen		= 9;
    static const int dtdHeaderLen		= 2;
    static const int elementHeaderLen	= 1;

    TIXMLASSERT( sizeof( XMLComment ) == sizeof( XMLUnknown ) );		// use same memory pool
    TIXMLASSERT( sizeof( XMLComment ) == sizeof( XMLDeclaration ) );	// use same memory pool
    XMLNode* returnNode = 0;
    if ( XMLUtil::StringEqual( p, xmlHeader, xmlHeaderLen ) ) {
        TIXMLASSERT( sizeof( XMLDeclaration ) == _commentPool.ItemSize() );
        returnNode = new (_commentPool.Alloc()) XMLDeclaration( this );
        returnNode->_memPool = &_commentPool;
        p += xmlHeaderLen;
    }
    else if ( XMLUtil::StringEqual( p, commentHeader, commentHeaderLen ) ) {
        TIXMLASSERT( sizeof( XMLComment ) == _commentPool.ItemSize() );
        returnNode = new (_commentPool.Alloc()) XMLComment( this );
        returnNode->_memPool = &_commentPool;
        p += commentHeaderLen;
    }
    else if ( XMLUtil::StringEqual( p, cdataHeader, cdataHeaderLen ) ) {
        TIXMLASSERT( sizeof( XMLText ) == _textPool.ItemSize() );
        XMLText* text = new (_textPool.Alloc()) XMLText( this );
        returnNode = text;
        returnNode->_memPool = &_textPool;
        p += cdataHeaderLen;
        text->SetCData( true );
    }
    else if ( XMLUtil::StringEqual( p, dtdHeader, dtdHeaderLen ) ) {
        TIXMLASSERT( sizeof( XMLUnknown ) == _commentPool.ItemSize() );
        returnNode = new (_commentPool.Alloc()) XMLUnknown( this );
        returnNode->_memPool = &_commentPool;
        p += dtdHeaderLen;
    }
    else if ( XMLUtil::StringEqual( p, elementHeader, elementHeaderLen ) ) {
        TIXMLASSERT( sizeof( XMLElement ) == _elementPool.ItemSize() );
        returnNode = new (_elementPool.Alloc()) XMLElement( this );
        returnNode->_memPool = &_elementPool;
        p += elementHeaderLen;
    }
    else {
        TIXMLASSERT( sizeof( XMLText ) == _textPool.ItemSize() );
        returnNode = new (_textPool.Alloc()) XMLText( this );
        returnNode->_memPool = &_textPool;
        p = start;	// Back it up, all the text counts.
    }

    TIXMLASSERT( returnNode );
    TIXMLASSERT( p );
    *node = returnNode;
    return p;
}


bool XMLDocument::Accept( XMLVisitor* visitor ) const
{
    TIXMLASSERT( visitor );
    if ( visitor->VisitEnter( *this ) ) {
        for ( const XMLNode* node=FirstChild(); node; node=node->NextSibling() ) {
            if ( !node->Accept( visitor ) ) {
                break;
            }
        }
    }
    return visitor->VisitExit( *this );
}


// --------- XMLNode ----------- //

XMLNode::XMLNode( XMLDocument* doc ) :
    _document( doc ),
    _parent( 0 ),
    _firstChild( 0 ), _lastChild( 0 ),
    _prev( 0 ), _next( 0 ),
    _memPool( 0 )
{
}


XMLNode::~XMLNode()
{
    DeleteChildren();
    if ( _parent ) {
        _parent->Unlink( this );
    }
}

const char* XMLNode::Value() const 
{
    // Catch an edge case: XMLDocuments don't have a a Value. Carefully return nullptr.
    if ( this->ToDocument() )
        return 0;
    return _value.GetStr();
}

void XMLNode::SetValue( const char* str, bool staticMem )
{
    if ( staticMem ) {
        _value.SetInternedStr( str );
    }
    else {
        _value.SetStr( str );
    }
}


void XMLNode::DeleteChildren()
{
    while( _firstChild ) {
        TIXMLASSERT( _lastChild );
        TIXMLASSERT( _firstChild->_document == _document );
        XMLNode* node = _firstChild;
        Unlink( node );

        DeleteNode( node );
    }
    _firstChild = _lastChild = 0;
}


void XMLNode::Unlink( XMLNode* child )
{
    TIXMLASSERT( child );
    TIXMLASSERT( child->_document == _document );
    TIXMLASSERT( child->_parent == this );
    if ( child == _firstChild ) {
        _firstChild = _firstChild->_next;
    }
    if ( child == _lastChild ) {
        _lastChild = _lastChild->_prev;
    }

    if ( child->_prev ) {
        child->_prev->_next = child->_next;
    }
    if ( child->_next ) {
        child->_next->_prev = child->_prev;
    }
	child->_parent = 0;
}


void XMLNode::DeleteChild( XMLNode* node )
{
    TIXMLASSERT( node );
    TIXMLASSERT( node->_document == _document );
    TIXMLASSERT( node->_parent == this );
    DeleteNode( node );
}


XMLNode* XMLNode::InsertEndChild( XMLNode* addThis )
{
    TIXMLASSERT( addThis );
    if ( addThis->_document != _document ) {
        TIXMLASSERT( false );
        return 0;
    }
    InsertChildPreamble( addThis );

    if ( _lastChild ) {
        TIXMLASSERT( _firstChild );
        TIXMLASSERT( _lastChild->_next == 0 );
        _lastChild->_next = addThis;
        addThis->_prev = _lastChild;
        _lastChild = addThis;

        addThis->_next = 0;
    }
    else {
        TIXMLASSERT( _firstChild == 0 );
        _firstChild = _lastChild = addThis;

        addThis->_prev = 0;
        addThis->_next = 0;
    }
    addThis->_parent = this;
    return addThis;
}


XMLNode* XMLNode::InsertFirstChild( XMLNode* addThis )
{
    TIXMLASSERT( addThis );
    if ( addThis->_document != _document ) {
        TIXMLASSERT( false );
        return 0;
    }
    InsertChildPreamble( addThis );

    if ( _firstChild ) {
        TIXMLASSERT( _lastChild );
        TIXMLASSERT( _firstChild->_prev == 0 );

        _firstChild->_prev = addThis;
        addThis->_next = _firstChild;
        _firstChild = addThis;

        addThis->_prev = 0;
    }
    else {
        TIXMLASSERT( _lastChild == 0 );
        _firstChild = _lastChild = addThis;

        addThis->_prev = 0;
        addThis->_next = 0;
    }
    addThis->_parent = this;
    return addThis;
}


XMLNode* XMLNode::InsertAfterChild( XMLNode* afterThis, XMLNode* addThis )
{
    TIXMLASSERT( addThis );
    if ( addThis->_document != _document ) {
        TIXMLASSERT( false );
        return 0;
    }

    TIXMLASSERT( afterThis );

    if ( afterThis->_parent != this ) {
        TIXMLASSERT( false );
        return 0;
    }

    if ( afterThis->_next == 0 ) {
        // The last node or the only node.
        return InsertEndChild( addThis );
    }
    InsertChildPreamble( addThis );
    addThis->_prev = afterThis;
    addThis->_next = afterThis->_next;
    afterThis->_next->_prev = addThis;
    afterThis->_next = addThis;
    addThis->_parent = this;
    return addThis;
}




const XMLElement* XMLNode::FirstChildElement( const char* name ) const
{
    for( const XMLNode* node = _firstChild; node; node = node->_next ) {
        const XMLElement* element = node->ToElement();
        if ( element ) {
            if ( !name || XMLUtil::StringEqual( element->Name(), name ) ) {
                return element;
            }
        }
    }
    return 0;
}


const XMLElement* XMLNode::LastChildElement( const char* name ) const
{
    for( const XMLNode* node = _lastChild; node; node = node->_prev ) {
        const XMLElement* element = node->ToElement();
        if ( element ) {
            if ( !name || XMLUtil::StringEqual( element->Name(), name ) ) {
                return element;
            }
        }
    }
    return 0;
}


const XMLElement* XMLNode::NextSiblingElement( const char* name ) const
{
    for( const XMLNode* node = _next; node; node = node->_next ) {
        const XMLElement* element = node->ToElement();
        if ( element
                && (!name || XMLUtil::StringEqual( name, element->Name() ))) {
            return element;
        }
    }
    return 0;
}


const XMLElement* XMLNode::PreviousSiblingElement( const char* name ) const
{
    for( const XMLNode* node = _prev; node; node = node->_prev ) {
        const XMLElement* element = node->ToElement();
        if ( element
                && (!name || XMLUtil::StringEqual( name, element->Name() ))) {
            return element;
        }
    }
    return 0;
}


char* XMLNode::ParseDeep( char* p, StrPair* parentEnd )
{
    // This is a recursive method, but thinking about it "at the current level"
    // it is a pretty simple flat list:
    //		<foo/>
    //		<!-- comment -->
    //
    // With a special case:
    //		<foo>
    //		</foo>
    //		<!-- comment -->
    //
    // Where the closing element (/foo) *must* be the next thing after the opening
    // element, and the names must match. BUT the tricky bit is that the closing
    // element will be read by the child.
    //
    // 'endTag' is the end tag for this node, it is returned by a call to a child.
    // 'parentEnd' is the end tag for the parent, which is filled in and returned.

    while( p && *p ) {
        XMLNode* node = 0;

        p = _document->Identify( p, &node );
        if ( node == 0 ) {
            break;
        }

        StrPair endTag;
        p = node->ParseDeep( p, &endTag );
        if ( !p ) {
            DeleteNode( node );
            if ( !_document->Error() ) {
                _document->SetError( XML_ERROR_PARSING, 0, 0 );
            }
            break;
        }

        XMLDeclaration* decl = node->ToDeclaration();
        if ( decl ) {
                // A declaration can only be the first child of a document.
                // Set error, if document already has children.
                if ( !_document->NoChildren() ) {
                        _document->SetError( XML_ERROR_PARSING_DECLARATION, decl->Value(), 0);
                        DeleteNode( decl );
                        break;
                }
        }

        XMLElement* ele = node->ToElement();
        if ( ele ) {
            // We read the end tag. Return it to the parent.
            if ( ele->ClosingType() == XMLElement::CLOSING ) {
                if ( parentEnd ) {
                    ele->_value.TransferTo( parentEnd );
                }
                node->_memPool->SetTracked();   // created and then immediately deleted.
                DeleteNode( node );
                return p;
            }

            // Handle an end tag returned to this level.
            // And handle a bunch of annoying errors.
            bool mismatch = false;
            if ( endTag.Empty() ) {
                if ( ele->ClosingType() == XMLElement::OPEN ) {
                    mismatch = true;
                }
            }
            else {
                if ( ele->ClosingType() != XMLElement::OPEN ) {
                    mismatch = true;
                }
                else if ( !XMLUtil::StringEqual( endTag.GetStr(), ele->Name() ) ) {
                    mismatch = true;
                }
            }
            if ( mismatch ) {
                _document->SetError( XML_ERROR_MISMATCHED_ELEMENT, ele->Name(), 0 );
                DeleteNode( node );
                break;
            }
        }
        InsertEndChild( node );
    }
    return 0;
}

void XMLNode::DeleteNode( XMLNode* node )
{
    if ( node == 0 ) {
        return;
    }
    MemPool* pool = node->_memPool;
    node->~XMLNode();
    pool->Free( node );
}

void XMLNode::InsertChildPreamble( XMLNode* insertThis ) const
{
    TIXMLASSERT( insertThis );
    TIXMLASSERT( insertThis->_document == _document );

    if ( insertThis->_parent )
        insertThis->_parent->Unlink( insertThis );
    else
        insertThis->_memPool->SetTracked();
}

// --------- XMLText ---------- //
char* XMLText::ParseDeep( char* p, StrPair* )
{
    const char* start = p;
    if ( this->CData() ) {
        p = _value.ParseText( p, "]]>", StrPair::NEEDS_NEWLINE_NORMALIZATION );
        if ( !p ) {
            _document->SetError( XML_ERROR_PARSING_CDATA, start, 0 );
        }
        return p;
    }
    else {
        int flags = _document->ProcessEntities() ? StrPair::TEXT_ELEMENT : StrPair::TEXT_ELEMENT_LEAVE_ENTITIES;
        if ( _document->WhitespaceMode() == COLLAPSE_WHITESPACE ) {
            flags |= StrPair::NEEDS_WHITESPACE_COLLAPSING;
        }

        p = _value.ParseText( p, "<", flags );
        if ( p && *p ) {
            return p-1;
        }
        if ( !p ) {
            _document->SetError( XML_ERROR_PARSING_TEXT, start, 0 );
        }
    }
    return 0;
}


XMLNode* XMLText::ShallowClone( XMLDocument* doc ) const
{
    if ( !doc ) {
        doc = _document;
    }
    XMLText* text = doc->NewText( Value() );	// fixme: this will always allocate memory. Intern?
    text->SetCData( this->CData() );
    return text;
}


bool XMLText::ShallowEqual( const XMLNode* compare ) const
{
    const XMLText* text = compare->ToText();
    return ( text && XMLUtil::StringEqual( text->Value(), Value() ) );
}


bool XMLText::Accept( XMLVisitor* visitor ) const
{
    TIXMLASSERT( visitor );
    return visitor->Visit( *this );
}


// --------- XMLComment ---------- //

XMLComment::XMLComment( XMLDocument* doc ) : XMLNode( doc )
{
}


XMLComment::~XMLComment()
{
}


char* XMLComment::ParseDeep( char* p, StrPair* )
{
    // Comment parses as text.
    const char* start = p;
    p = _value.ParseText( p, "-->", StrPair::COMMENT );
    if ( p == 0 ) {
        _document->SetError( XML_ERROR_PARSING_COMMENT, start, 0 );
    }
    return p;
}


XMLNode* XMLComment::ShallowClone( XMLDocument* doc ) const
{
    if ( !doc ) {
        doc = _document;
    }
    XMLComment* comment = doc->NewComment( Value() );	// fixme: this will always allocate memory. Intern?
    return comment;
}


bool XMLComment::ShallowEqual( const XMLNode* compare ) const
{
    TIXMLASSERT( compare );
    const XMLComment* comment = compare->ToComment();
    return ( comment && XMLUtil::StringEqual( comment->Value(), Value() ));
}


bool XMLComment::Accept( XMLVisitor* visitor ) const
{
    TIXMLASSERT( visitor );
    return visitor->Visit( *this );
}


// --------- XMLDeclaration ---------- //

XMLDeclaration::XMLDeclaration( XMLDocument* doc ) : XMLNode( doc )
{
}


XMLDeclaration::~XMLDeclaration()
{
    //printf( "~XMLDeclaration\n" );
}


char* XMLDeclaration::ParseDeep( char* p, StrPair* )
{
    // Declaration parses as text.
    const char* start = p;
    p = _value.ParseText( p, "?>", StrPair::NEEDS_NEWLINE_NORMALIZATION );
    if ( p == 0 ) {
        _document->SetError( XML_ERROR_PARSING_DECLARATION, start, 0 );
    }
    return p;
}


XMLNode* XMLDeclaration::ShallowClone( XMLDocument* doc ) const
{
    if ( !doc ) {
        doc = _document;
    }
    XMLDeclaration* dec = doc->NewDeclaration( Value() );	// fixme: this will always allocate memory. Intern?
    return dec;
}


bool XMLDeclaration::ShallowEqual( const XMLNode* compare ) const
{
    TIXMLASSERT( compare );
    const XMLDeclaration* declaration = compare->ToDeclaration();
    return ( declaration && XMLUtil::StringEqual( declaration->Value(), Value() ));
}



bool XMLDeclaration::Accept( XMLVisitor* visitor ) const
{
    TIXMLASSERT( visitor );
    return visitor->Visit( *this );
}

// --------- XMLUnknown ---------- //

XMLUnknown::XMLUnknown( XMLDocument* doc ) : XMLNode( doc )
{
}


XMLUnknown::~XMLUnknown()
{
}


char* XMLUnknown::ParseDeep( char* p, StrPair* )
{
    // Unknown parses as text.
    const char* start = p;

    p = _value.ParseText( p, ">", StrPair::NEEDS_NEWLINE_NORMALIZATION );
    if ( !p ) {
        _document->SetError( XML_ERROR_PARSING_UNKNOWN, start, 0 );
    }
    return p;
}


XMLNode* XMLUnknown::ShallowClone( XMLDocument* doc ) const
{
    if ( !doc ) {
        doc = _document;
    }
    XMLUnknown* text = doc->NewUnknown( Value() );	// fixme: this will always allocate memory. Intern?
    return text;
}


bool XMLUnknown::ShallowEqual( const XMLNode* compare ) const
{
    TIXMLASSERT( compare );
    const XMLUnknown* unknown = compare->ToUnknown();
    return ( unknown && XMLUtil::StringEqual( unknown->Value(), Value() ));
}


bool XMLUnknown::Accept( XMLVisitor* visitor ) const
{
    TIXMLASSERT( visitor );
    return visitor->Visit( *this );
}

// --------- XMLAttribute ---------- //

const char* XMLAttribute::Name() const 
{
    return _name.GetStr();
}

const char* XMLAttribute::Value() const 
{
    return _value.GetStr();
}

char* XMLAttribute::ParseDeep( char* p, bool processEntities )
{
    // Parse using the name rules: bug fix, was using ParseText before
    p = _name.ParseName( p );
    if ( !p || !*p ) {
        return 0;
    }

    // Skip white space before =
    p = XMLUtil::SkipWhiteSpace( p );
    if ( *p != '=' ) {
        return 0;
    }

    ++p;	// move up to opening quote
    p = XMLUtil::SkipWhiteSpace( p );
    if ( *p != '\"' && *p != '\'' ) {
        return 0;
    }

    char endTag[2] = { *p, 0 };
    ++p;	// move past opening quote

    p = _value.ParseText( p, endTag, processEntities ? StrPair::ATTRIBUTE_VALUE : StrPair::ATTRIBUTE_VALUE_LEAVE_ENTITIES );
    return p;
}


void XMLAttribute::SetName( const char* n )
{
    _name.SetStr( n );
}


XMLError XMLAttribute::QueryIntValue( int* value ) const
{
    if ( XMLUtil::ToInt( Value(), value )) {
        return XML_NO_ERROR;
    }
    return XML_WRONG_ATTRIBUTE_TYPE;
}


XMLError XMLAttribute::QueryUnsignedValue( unsigned int* value ) const
{
    if ( XMLUtil::ToUnsigned( Value(), value )) {
        return XML_NO_ERROR;
    }
    return XML_WRONG_ATTRIBUTE_TYPE;
}


XMLError XMLAttribute::QueryBoolValue( bool* value ) const
{
    if ( XMLUtil::ToBool( Value(), value )) {
        return XML_NO_ERROR;
    }
    return XML_WRONG_ATTRIBUTE_TYPE;
}


XMLError XMLAttribute::QueryFloatValue( float* value ) const
{
    if ( XMLUtil::ToFloat( Value(), value )) {
        return XML_NO_ERROR;
    }
    return XML_WRONG_ATTRIBUTE_TYPE;
}


XMLError XMLAttribute::QueryDoubleValue( double* value ) const
{
    if ( XMLUtil::ToDouble( Value(), value )) {
        return XML_NO_ERROR;
    }
    return XML_WRONG_ATTRIBUTE_TYPE;
}


void XMLAttribute::SetAttribute( const char* v )
{
    _value.SetStr( v );
}


void XMLAttribute::SetAttribute( int v )
{
    char buf[BUF_SIZE];
    XMLUtil::ToStr( v, buf, BUF_SIZE );
    _value.SetStr( buf );
}


void XMLAttribute::SetAttribute( unsigned v )
{
    char buf[BUF_SIZE];
    XMLUtil::ToStr( v, buf, BUF_SIZE );
    _value.SetStr( buf );
}


void XMLAttribute::SetAttribute( bool v )
{
    char buf[BUF_SIZE];
    XMLUtil::ToStr( v, buf, BUF_SIZE );
    _value.SetStr( buf );
}

void XMLAttribute::SetAttribute( double v )
{
    char buf[BUF_SIZE];
    XMLUtil::ToStr( v, buf, BUF_SIZE );
    _value.SetStr( buf );
}

void XMLAttribute::SetAttribute( float v )
{
    char buf[BUF_SIZE];
    XMLUtil::ToStr( v, buf, BUF_SIZE );
    _value.SetStr( buf );
}


// --------- XMLElement ---------- //
XMLElement::XMLElement( XMLDocument* doc ) : XMLNode( doc ),
    _closingType( 0 ),
    _rootAttribute( 0 )
{
}


XMLElement::~XMLElement()
{
    while( _rootAttribute ) {
        XMLAttribute* next = _rootAttribute->_next;
        DeleteAttribute( _rootAttribute );
        _rootAttribute = next;
    }
}


const XMLAttribute* XMLElement::FindAttribute( const char* name ) const
{
    for( XMLAttribute* a = _rootAttribute; a; a = a->_next ) {
        if ( XMLUtil::StringEqual( a->Name(), name ) ) {
            return a;
        }
    }
    return 0;
}


const char* XMLElement::Attribute( const char* name, const char* value ) const
{
    const XMLAttribute* a = FindAttribute( name );
    if ( !a ) {
        return 0;
    }
    if ( !value || XMLUtil::StringEqual( a->Value(), value )) {
        return a->Value();
    }
    return 0;
}


const char* XMLElement::GetText() const
{
    if ( FirstChild() && FirstChild()->ToText() ) {
        return FirstChild()->Value();
    }
    return 0;
}


void	XMLElement::SetText( const char* inText )
{
	if ( FirstChild() && FirstChild()->ToText() )
		FirstChild()->SetValue( inText );
	else {
		XMLText*	theText = GetDocument()->NewText( inText );
		InsertFirstChild( theText );
	}
}


void XMLElement::SetText( int v ) 
{
    char buf[BUF_SIZE];
    XMLUtil::ToStr( v, buf, BUF_SIZE );
    SetText( buf );
}


void XMLElement::SetText( unsigned v ) 
{
    char buf[BUF_SIZE];
    XMLUtil::ToStr( v, buf, BUF_SIZE );
    SetText( buf );
}


void XMLElement::SetText( bool v ) 
{
    char buf[BUF_SIZE];
    XMLUtil::ToStr( v, buf, BUF_SIZE );
    SetText( buf );
}


void XMLElement::SetText( float v ) 
{
    char buf[BUF_SIZE];
    XMLUtil::ToStr( v, buf, BUF_SIZE );
    SetText( buf );
}


void XMLElement::SetText( double v ) 
{
    char buf[BUF_SIZE];
    XMLUtil::ToStr( v, buf, BUF_SIZE );
    SetText( buf );
}


XMLError XMLElement::QueryIntText( int* ival ) const
{
    if ( FirstChild() && FirstChild()->ToText() ) {
        const char* t = FirstChild()->Value();
        if ( XMLUtil::ToInt( t, ival ) ) {
            return XML_SUCCESS;
        }
        return XML_CAN_NOT_CONVERT_TEXT;
    }
    return XML_NO_TEXT_NODE;
}


XMLError XMLElement::QueryUnsignedText( unsigned* uval ) const
{
    if ( FirstChild() && FirstChild()->ToText() ) {
        const char* t = FirstChild()->Value();
        if ( XMLUtil::ToUnsigned( t, uval ) ) {
            return XML_SUCCESS;
        }
        return XML_CAN_NOT_CONVERT_TEXT;
    }
    return XML_NO_TEXT_NODE;
}


XMLError XMLElement::QueryBoolText( bool* bval ) const
{
    if ( FirstChild() && FirstChild()->ToText() ) {
        const char* t = FirstChild()->Value();
        if ( XMLUtil::ToBool( t, bval ) ) {
            return XML_SUCCESS;
        }
        return XML_CAN_NOT_CONVERT_TEXT;
    }
    return XML_NO_TEXT_NODE;
}


XMLError XMLElement::QueryDoubleText( double* dval ) const
{
    if ( FirstChild() && FirstChild()->ToText() ) {
        const char* t = FirstChild()->Value();
        if ( XMLUtil::ToDouble( t, dval ) ) {
            return XML_SUCCESS;
        }
        return XML_CAN_NOT_CONVERT_TEXT;
    }
    return XML_NO_TEXT_NODE;
}


XMLError XMLElement::QueryFloatText( float* fval ) const
{
    if ( FirstChild() && FirstChild()->ToText() ) {
        const char* t = FirstChild()->Value();
        if ( XMLUtil::ToFloat( t, fval ) ) {
            return XML_SUCCESS;
        }
        return XML_CAN_NOT_CONVERT_TEXT;
    }
    return XML_NO_TEXT_NODE;
}



XMLAttribute* XMLElement::FindOrCreateAttribute( const char* name )
{
    XMLAttribute* last = 0;
    XMLAttribute* attrib = 0;
    for( attrib = _rootAttribute;
            attrib;
            last = attrib, attrib = attrib->_next ) {
        if ( XMLUtil::StringEqual( attrib->Name(), name ) ) {
            break;
        }
    }
    if ( !attrib ) {
        TIXMLASSERT( sizeof( XMLAttribute ) == _document->_attributePool.ItemSize() );
        attrib = new (_document->_attributePool.Alloc() ) XMLAttribute();
        attrib->_memPool = &_document->_attributePool;
        if ( last ) {
            last->_next = attrib;
        }
        else {
            _rootAttribute = attrib;
        }
        attrib->SetName( name );
        attrib->_memPool->SetTracked(); // always created and linked.
    }
    return attrib;
}


void XMLElement::DeleteAttribute( const char* name )
{
    XMLAttribute* prev = 0;
    for( XMLAttribute* a=_rootAttribute; a; a=a->_next ) {
        if ( XMLUtil::StringEqual( name, a->Name() ) ) {
            if ( prev ) {
                prev->_next = a->_next;
            }
            else {
                _rootAttribute = a->_next;
            }
            DeleteAttribute( a );
            break;
        }
        prev = a;
    }
}


char* XMLElement::ParseAttributes( char* p )
{
    const char* start = p;
    XMLAttribute* prevAttribute = 0;

    // Read the attributes.
    while( p ) {
        p = XMLUtil::SkipWhiteSpace( p );
        if ( !(*p) ) {
            _document->SetError( XML_ERROR_PARSING_ELEMENT, start, Name() );
            return 0;
        }

        // attribute.
        if (XMLUtil::IsNameStartChar( *p ) ) {
            TIXMLASSERT( sizeof( XMLAttribute ) == _document->_attributePool.ItemSize() );
            XMLAttribute* attrib = new (_document->_attributePool.Alloc() ) XMLAttribute();
            attrib->_memPool = &_document->_attributePool;
			attrib->_memPool->SetTracked();

            p = attrib->ParseDeep( p, _document->ProcessEntities() );
            if ( !p || Attribute( attrib->Name() ) ) {
                DeleteAttribute( attrib );
                _document->SetError( XML_ERROR_PARSING_ATTRIBUTE, start, p );
                return 0;
            }
            // There is a minor bug here: if the attribute in the source xml
            // document is duplicated, it will not be detected and the
            // attribute will be doubly added. However, tracking the 'prevAttribute'
            // avoids re-scanning the attribute list. Preferring performance for
            // now, may reconsider in the future.
            if ( prevAttribute ) {
                prevAttribute->_next = attrib;
            }
            else {
                _rootAttribute = attrib;
            }
            prevAttribute = attrib;
        }
        // end of the tag
        else if ( *p == '>' ) {
            ++p;
            break;
        }
        // end of the tag
        else if ( *p == '/' && *(p+1) == '>' ) {
            _closingType = CLOSED;
            return p+2;	// done; sealed element.
        }
        else {
            _document->SetError( XML_ERROR_PARSING_ELEMENT, start, p );
            return 0;
        }
    }
    return p;
}

void XMLElement::DeleteAttribute( XMLAttribute* attribute )
{
    if ( attribute == 0 ) {
        return;
    }
    MemPool* pool = attribute->_memPool;
    attribute->~XMLAttribute();
    pool->Free( attribute );
}

//
//	<ele></ele>
//	<ele>foo<b>bar</b></ele>
//
char* XMLElement::ParseDeep( char* p, StrPair* strPair )
{
    // Read the element name.
    p = XMLUtil::SkipWhiteSpace( p );

    // The closing element is the </element> form. It is
    // parsed just like a regular element then deleted from
    // the DOM.
    if ( *p == '/' ) {
        _closingType = CLOSING;
        ++p;
    }

    p = _value.ParseName( p );
    if ( _value.Empty() ) {
        return 0;
    }

    p = ParseAttributes( p );
    if ( !p || !*p || _closingType ) {
        return p;
    }

    p = XMLNode::ParseDeep( p, strPair );
    return p;
}



XMLNode* XMLElement::ShallowClone( XMLDocument* doc ) const
{
    if ( !doc ) {
        doc = _document;
    }
    XMLElement* element = doc->NewElement( Value() );					// fixme: this will always allocate memory. Intern?
    for( const XMLAttribute* a=FirstAttribute(); a; a=a->Next() ) {
        element->SetAttribute( a->Name(), a->Value() );					// fixme: this will always allocate memory. Intern?
    }
    return element;
}


bool XMLElement::ShallowEqual( const XMLNode* compare ) const
{
    TIXMLASSERT( compare );
    const XMLElement* other = compare->ToElement();
    if ( other && XMLUtil::StringEqual( other->Name(), Name() )) {

        const XMLAttribute* a=FirstAttribute();
        const XMLAttribute* b=other->FirstAttribute();

        while ( a && b ) {
            if ( !XMLUtil::StringEqual( a->Value(), b->Value() ) ) {
                return false;
            }
            a = a->Next();
            b = b->Next();
        }
        if ( a || b ) {
            // different count
            return false;
        }
        return true;
    }
    return false;
}


bool XMLElement::Accept( XMLVisitor* visitor ) const
{
    TIXMLASSERT( visitor );
    if ( visitor->VisitEnter( *this, _rootAttribute ) ) {
        for ( const XMLNode* node=FirstChild(); node; node=node->NextSibling() ) {
            if ( !node->Accept( visitor ) ) {
                break;
            }
        }
    }
    return visitor->VisitExit( *this );
}


// --------- XMLDocument ----------- //

// Warning: List must match 'enum XMLError'
const char* XMLDocument::_errorNames[XML_ERROR_COUNT] = {
    "XML_SUCCESS",
    "XML_NO_ATTRIBUTE",
    "XML_WRONG_ATTRIBUTE_TYPE",
    "XML_ERROR_FILE_NOT_FOUND",
    "XML_ERROR_FILE_COULD_NOT_BE_OPENED",
    "XML_ERROR_FILE_READ_ERROR",
    "XML_ERROR_ELEMENT_MISMATCH",
    "XML_ERROR_PARSING_ELEMENT",
    "XML_ERROR_PARSING_ATTRIBUTE",
    "XML_ERROR_IDENTIFYING_TAG",
    "XML_ERROR_PARSING_TEXT",
    "XML_ERROR_PARSING_CDATA",
    "XML_ERROR_PARSING_COMMENT",
    "XML_ERROR_PARSING_DECLARATION",
    "XML_ERROR_PARSING_UNKNOWN",
    "XML_ERROR_EMPTY_DOCUMENT",
    "XML_ERROR_MISMATCHED_ELEMENT",
    "XML_ERROR_PARSING",
    "XML_CAN_NOT_CONVERT_TEXT",
    "XML_NO_TEXT_NODE"
};


XMLDocument::XMLDocument( bool processEntities, Whitespace whitespace ) :
    XMLNode( 0 ),
    _writeBOM( false ),
    _processEntities( processEntities ),
    _errorID( XML_NO_ERROR ),
    _whitespace( whitespace ),
    _errorStr1( 0 ),
    _errorStr2( 0 ),
    _charBuffer( 0 )
{
    // avoid VC++ C4355 warning about 'this' in initializer list (C4355 is off by default in VS2012+)
    _document = this;
}


XMLDocument::~XMLDocument()
{
    Clear();
}


void XMLDocument::Clear()
{
    DeleteChildren();

#ifdef DEBUG
    const bool hadError = Error();
#endif
    _errorID = XML_NO_ERROR;
    _errorStr1 = 0;
    _errorStr2 = 0;

    delete [] _charBuffer;
    _charBuffer = 0;

#if 0
    _textPool.Trace( "text" );
    _elementPool.Trace( "element" );
    _commentPool.Trace( "comment" );
    _attributePool.Trace( "attribute" );
#endif
    
#ifdef DEBUG
    if ( !hadError ) {
        TIXMLASSERT( _elementPool.CurrentAllocs()   == _elementPool.Untracked() );
        TIXMLASSERT( _attributePool.CurrentAllocs() == _attributePool.Untracked() );
        TIXMLASSERT( _textPool.CurrentAllocs()      == _textPool.Untracked() );
        TIXMLASSERT( _commentPool.CurrentAllocs()   == _commentPool.Untracked() );
    }
#endif
}


XMLElement* XMLDocument::NewElement( const char* name )
{
    TIXMLASSERT( sizeof( XMLElement ) == _elementPool.ItemSize() );
    XMLElement* ele = new (_elementPool.Alloc()) XMLElement( this );
    ele->_memPool = &_elementPool;
    ele->SetName( name );
    return ele;
}


XMLComment* XMLDocument::NewComment( const char* str )
{
    TIXMLASSERT( sizeof( XMLComment ) == _commentPool.ItemSize() );
    XMLComment* comment = new (_commentPool.Alloc()) XMLComment( this );
    comment->_memPool = &_commentPool;
    comment->SetValue( str );
    return comment;
}


XMLText* XMLDocument::NewText( const char* str )
{
    TIXMLASSERT( sizeof( XMLText ) == _textPool.ItemSize() );
    XMLText* text = new (_textPool.Alloc()) XMLText( this );
    text->_memPool = &_textPool;
    text->SetValue( str );
    return text;
}


XMLDeclaration* XMLDocument::NewDeclaration( const char* str )
{
    TIXMLASSERT( sizeof( XMLDeclaration ) == _commentPool.ItemSize() );
    XMLDeclaration* dec = new (_commentPool.Alloc()) XMLDeclaration( this );
    dec->_memPool = &_commentPool;
    dec->SetValue( str ? str : "xml version=\"1.0\" encoding=\"UTF-8\"" );
    return dec;
}


XMLUnknown* XMLDocument::NewUnknown( const char* str )
{
    TIXMLASSERT( sizeof( XMLUnknown ) == _commentPool.ItemSize() );
    XMLUnknown* unk = new (_commentPool.Alloc()) XMLUnknown( this );
    unk->_memPool = &_commentPool;
    unk->SetValue( str );
    return unk;
}

static FILE* callfopen( const char* filepath, const char* mode )
{
    TIXMLASSERT( filepath );
    TIXMLASSERT( mode );
#if defined(_MSC_VER) && (_MSC_VER >= 1400 ) && (!defined WINCE)
    FILE* fp = 0;
    errno_t err = fopen_s( &fp, filepath, mode );
    if ( err ) {
        return 0;
    }
#else
    FILE* fp = fopen( filepath, mode );
#endif
    return fp;
}
    
void XMLDocument::DeleteNode( XMLNode* node )	{
    TIXMLASSERT( node );
    TIXMLASSERT(node->_document == this );
    if (node->_parent) {
        node->_parent->DeleteChild( node );
    }
    else {
        // Isn't in the tree.
        // Use the parent delete.
        // Also, we need to mark it tracked: we 'know'
        // it was never used.
        node->_memPool->SetTracked();
        // Call the static XMLNode version:
        XMLNode::DeleteNode(node);
    }
}


XMLError XMLDocument::LoadFile( const char* filename )
{
    Clear();
    FILE* fp = callfopen( filename, "rb" );
    if ( !fp ) {
        SetError( XML_ERROR_FILE_NOT_FOUND, filename, 0 );
        return _errorID;
    }
    LoadFile( fp );
    fclose( fp );
    return _errorID;
}


XMLError XMLDocument::LoadFile( FILE* fp )
{
    Clear();

    fseek( fp, 0, SEEK_SET );
    if ( fgetc( fp ) == EOF && ferror( fp ) != 0 ) {
        SetError( XML_ERROR_FILE_READ_ERROR, 0, 0 );
        return _errorID;
    }

    fseek( fp, 0, SEEK_END );
    const long filelength = ftell( fp );
    fseek( fp, 0, SEEK_SET );
    if ( filelength == -1L ) {
        SetError( XML_ERROR_FILE_READ_ERROR, 0, 0 );
        return _errorID;
    }

    if ( (unsigned long)filelength >= (size_t)-1 ) {
        // Cannot handle files which won't fit in buffer together with null terminator
        SetError( XML_ERROR_FILE_READ_ERROR, 0, 0 );
        return _errorID;
    }

    if ( filelength == 0 ) {
        SetError( XML_ERROR_EMPTY_DOCUMENT, 0, 0 );
        return _errorID;
    }

    const size_t size = filelength;
    TIXMLASSERT( _charBuffer == 0 );
    _charBuffer = new char[size+1];
    size_t read = fread( _charBuffer, 1, size, fp );
    if ( read != size ) {
        SetError( XML_ERROR_FILE_READ_ERROR, 0, 0 );
        return _errorID;
    }

    _charBuffer[size] = 0;

    Parse();
    return _errorID;
}


XMLError XMLDocument::SaveFile( const char* filename, bool compact )
{
    FILE* fp = callfopen( filename, "w" );
    if ( !fp ) {
        SetError( XML_ERROR_FILE_COULD_NOT_BE_OPENED, filename, 0 );
        return _errorID;
    }
    SaveFile(fp, compact);
    fclose( fp );
    return _errorID;
}


XMLError XMLDocument::SaveFile( FILE* fp, bool compact )
{
    // Clear any error from the last save, otherwise it will get reported
    // for *this* call.
    SetError( XML_NO_ERROR, 0, 0 );
    XMLPrinter stream( fp, compact );
    Print( &stream );
    return _errorID;
}


XMLError XMLDocument::Parse( const char* p, size_t len )
{
    Clear();

    if ( len == 0 || !p || !*p ) {
        SetError( XML_ERROR_EMPTY_DOCUMENT, 0, 0 );
        return _errorID;
    }
    if ( len == (size_t)(-1) ) {
        len = strlen( p );
    }
    TIXMLASSERT( _charBuffer == 0 );
    _charBuffer = new char[ len+1 ];
    memcpy( _charBuffer, p, len );
    _charBuffer[len] = 0;

    Parse();
    if ( Error() ) {
        // clean up now essentially dangling memory.
        // and the parse fail can put objects in the
        // pools that are dead and inaccessible.
        DeleteChildren();
        _elementPool.Clear();
        _attributePool.Clear();
        _textPool.Clear();
        _commentPool.Clear();
    }
    return _errorID;
}


void XMLDocument::Print( XMLPrinter* streamer ) const
{
    if ( streamer ) {
        Accept( streamer );
    }
    else {
        XMLPrinter stdoutStreamer( stdout );
        Accept( &stdoutStreamer );
    }
}


void XMLDocument::SetError( XMLError error, const char* str1, const char* str2 )
{
    TIXMLASSERT( error >= 0 && error < XML_ERROR_COUNT );
    _errorID = error;
    _errorStr1 = str1;
    _errorStr2 = str2;
}

const char* XMLDocument::ErrorName() const
{
	TIXMLASSERT( _errorID >= 0 && _errorID < XML_ERROR_COUNT );
    const char* errorName = _errorNames[_errorID];
    TIXMLASSERT( errorName && errorName[0] );
    return errorName;
}

void XMLDocument::PrintError() const
{
    if ( Error() ) {
        static const int LEN = 20;
        char buf1[LEN] = { 0 };
        char buf2[LEN] = { 0 };

        if ( _errorStr1 ) {
            TIXML_SNPRINTF( buf1, LEN, "%s", _errorStr1 );
        }
        if ( _errorStr2 ) {
            TIXML_SNPRINTF( buf2, LEN, "%s", _errorStr2 );
        }

        // Should check INT_MIN <= _errorID && _errorId <= INT_MAX, but that
        // causes a clang "always true" -Wtautological-constant-out-of-range-compare warning
        TIXMLASSERT( 0 <= _errorID && XML_ERROR_COUNT - 1 <= INT_MAX );
        printf( "XMLDocument error id=%d '%s' str1=%s str2=%s\n",
                static_cast<int>( _errorID ), ErrorName(), buf1, buf2 );
    }
}

void XMLDocument::Parse()
{
    TIXMLASSERT( NoChildren() ); // Clear() must have been called previously
    TIXMLASSERT( _charBuffer );
    char* p = _charBuffer;
    p = XMLUtil::SkipWhiteSpace( p );
    p = const_cast<char*>( XMLUtil::ReadBOM( p, &_writeBOM ) );
    if ( !*p ) {
        SetError( XML_ERROR_EMPTY_DOCUMENT, 0, 0 );
        return;
    }
    ParseDeep(p, 0 );
}

XMLPrinter::XMLPrinter( FILE* file, bool compact, int depth ) :
    _elementJustOpened( false ),
    _firstElement( true ),
    _fp( file ),
    _depth( depth ),
    _textDepth( -1 ),
    _processEntities( true ),
    _compactMode( compact )
{
    for( int i=0; i<ENTITY_RANGE; ++i ) {
        _entityFlag[i] = false;
        _restrictedEntityFlag[i] = false;
    }
    for( int i=0; i<NUM_ENTITIES; ++i ) {
        const char entityValue = entities[i].value;
        TIXMLASSERT( 0 <= entityValue && entityValue < ENTITY_RANGE );
        _entityFlag[ (unsigned char)entityValue ] = true;
    }
    _restrictedEntityFlag[(unsigned char)'&'] = true;
    _restrictedEntityFlag[(unsigned char)'<'] = true;
    _restrictedEntityFlag[(unsigned char)'>'] = true;	// not required, but consistency is nice
    _buffer.Push( 0 );
}


void XMLPrinter::Print( const char* format, ... )
{
    va_list     va;
    va_start( va, format );

    if ( _fp ) {
        vfprintf( _fp, format, va );
    }
    else {
        const int len = TIXML_VSCPRINTF( format, va );
        // Close out and re-start the va-args
        va_end( va );
        TIXMLASSERT( len >= 0 );
        va_start( va, format );
        TIXMLASSERT( _buffer.Size() > 0 && _buffer[_buffer.Size() - 1] == 0 );
        char* p = _buffer.PushArr( len ) - 1;	// back up over the null terminator.
		TIXML_VSNPRINTF( p, len+1, format, va );
    }
    va_end( va );
}


void XMLPrinter::PrintSpace( int depth )
{
    for( int i=0; i<depth; ++i ) {
        Print( "    " );
    }
}


void XMLPrinter::PrintString( const char* p, bool restricted )
{
    // Look for runs of bytes between entities to print.
    const char* q = p;

    if ( _processEntities ) {
        const bool* flag = restricted ? _restrictedEntityFlag : _entityFlag;
        while ( *q ) {
            TIXMLASSERT( p <= q );
            // Remember, char is sometimes signed. (How many times has that bitten me?)
            if ( *q > 0 && *q < ENTITY_RANGE ) {
                // Check for entities. If one is found, flush
                // the stream up until the entity, write the
                // entity, and keep looking.
                if ( flag[(unsigned char)(*q)] ) {
                    while ( p < q ) {
                        const size_t delta = q - p;
                        // %.*s accepts type int as "precision"
                        const int toPrint = ( INT_MAX < delta ) ? INT_MAX : (int)delta;
                        Print( "%.*s", toPrint, p );
                        p += toPrint;
                    }
                    bool entityPatternPrinted = false;
                    for( int i=0; i<NUM_ENTITIES; ++i ) {
                        if ( entities[i].value == *q ) {
                            Print( "&%s;", entities[i].pattern );
                            entityPatternPrinted = true;
                            break;
                        }
                    }
                    if ( !entityPatternPrinted ) {
                        // TIXMLASSERT( entityPatternPrinted ) causes gcc -Wunused-but-set-variable in release
                        TIXMLASSERT( false );
                    }
                    ++p;
                }
            }
            ++q;
            TIXMLASSERT( p <= q );
        }
    }
    // Flush the remaining string. This will be the entire
    // string if an entity wasn't found.
    TIXMLASSERT( p <= q );
    if ( !_processEntities || ( p < q ) ) {
        Print( "%s", p );
    }
}


void XMLPrinter::PushHeader( bool writeBOM, bool writeDec )
{
    if ( writeBOM ) {
        static const unsigned char bom[] = { TIXML_UTF_LEAD_0, TIXML_UTF_LEAD_1, TIXML_UTF_LEAD_2, 0 };
        Print( "%s", bom );
    }
    if ( writeDec ) {
        PushDeclaration( "xml version=\"1.0\"" );
    }
}


void XMLPrinter::OpenElement( const char* name, bool compactMode )
{
    SealElementIfJustOpened();
    _stack.Push( name );

    if ( _textDepth < 0 && !_firstElement && !compactMode ) {
        Print( "\n" );
    }
    if ( !compactMode ) {
        PrintSpace( _depth );
    }

    Print( "<%s", name );
    _elementJustOpened = true;
    _firstElement = false;
    ++_depth;
}


void XMLPrinter::PushAttribute( const char* name, const char* value )
{
    TIXMLASSERT( _elementJustOpened );
    Print( " %s=\"", name );
    PrintString( value, false );
    Print( "\"" );
}


void XMLPrinter::PushAttribute( const char* name, int v )
{
    char buf[BUF_SIZE];
    XMLUtil::ToStr( v, buf, BUF_SIZE );
    PushAttribute( name, buf );
}


void XMLPrinter::PushAttribute( const char* name, unsigned v )
{
    char buf[BUF_SIZE];
    XMLUtil::ToStr( v, buf, BUF_SIZE );
    PushAttribute( name, buf );
}


void XMLPrinter::PushAttribute( const char* name, bool v )
{
    char buf[BUF_SIZE];
    XMLUtil::ToStr( v, buf, BUF_SIZE );
    PushAttribute( name, buf );
}


void XMLPrinter::PushAttribute( const char* name, double v )
{
    char buf[BUF_SIZE];
    XMLUtil::ToStr( v, buf, BUF_SIZE );
    PushAttribute( name, buf );
}


void XMLPrinter::CloseElement( bool compactMode )
{
    --_depth;
    const char* name = _stack.Pop();

    if ( _elementJustOpened ) {
        Print( "/>" );
    }
    else {
        if ( _textDepth < 0 && !compactMode) {
            Print( "\n" );
            PrintSpace( _depth );
        }
        Print( "</%s>", name );
    }

    if ( _textDepth == _depth ) {
        _textDepth = -1;
    }
    if ( _depth == 0 && !compactMode) {
        Print( "\n" );
    }
    _elementJustOpened = false;
}


void XMLPrinter::SealElementIfJustOpened()
{
    if ( !_elementJustOpened ) {
        return;
    }
    _elementJustOpened = false;
    Print( ">" );
}


void XMLPrinter::PushText( const char* text, bool cdata )
{
    _textDepth = _depth-1;

    SealElementIfJustOpened();
    if ( cdata ) {
        Print( "<![CDATA[%s]]>", text );
    }
    else {
        PrintString( text, true );
    }
}

void XMLPrinter::PushText( int value )
{
    char buf[BUF_SIZE];
    XMLUtil::ToStr( value, buf, BUF_SIZE );
    PushText( buf, false );
}


void XMLPrinter::PushText( unsigned value )
{
    char buf[BUF_SIZE];
    XMLUtil::ToStr( value, buf, BUF_SIZE );
    PushText( buf, false );
}


void XMLPrinter::PushText( bool value )
{
    char buf[BUF_SIZE];
    XMLUtil::ToStr( value, buf, BUF_SIZE );
    PushText( buf, false );
}


void XMLPrinter::PushText( float value )
{
    char buf[BUF_SIZE];
    XMLUtil::ToStr( value, buf, BUF_SIZE );
    PushText( buf, false );
}


void XMLPrinter::PushText( double value )
{
    char buf[BUF_SIZE];
    XMLUtil::ToStr( value, buf, BUF_SIZE );
    PushText( buf, false );
}


void XMLPrinter::PushComment( const char* comment )
{
    SealElementIfJustOpened();
    if ( _textDepth < 0 && !_firstElement && !_compactMode) {
        Print( "\n" );
        PrintSpace( _depth );
    }
    _firstElement = false;
    Print( "<!--%s-->", comment );
}


void XMLPrinter::PushDeclaration( const char* value )
{
    SealElementIfJustOpened();
    if ( _textDepth < 0 && !_firstElement && !_compactMode) {
        Print( "\n" );
        PrintSpace( _depth );
    }
    _firstElement = false;
    Print( "<?%s?>", value );
}


void XMLPrinter::PushUnknown( const char* value )
{
    SealElementIfJustOpened();
    if ( _textDepth < 0 && !_firstElement && !_compactMode) {
        Print( "\n" );
        PrintSpace( _depth );
    }
    _firstElement = false;
    Print( "<!%s>", value );
}


bool XMLPrinter::VisitEnter( const XMLDocument& doc )
{
    _processEntities = doc.ProcessEntities();
    if ( doc.HasBOM() ) {
        PushHeader( true, false );
    }
    return true;
}


bool XMLPrinter::VisitEnter( const XMLElement& element, const XMLAttribute* attribute )
{
    const XMLElement* parentElem = 0;
    if ( element.Parent() ) {
        parentElem = element.Parent()->ToElement();
    }
    const bool compactMode = parentElem ? CompactMode( *parentElem ) : _compactMode;
    OpenElement( element.Name(), compactMode );
    while ( attribute ) {
        PushAttribute( attribute->Name(), attribute->Value() );
        attribute = attribute->Next();
    }
    return true;
}


bool XMLPrinter::VisitExit( const XMLElement& element )
{
    CloseElement( CompactMode(element) );
    return true;
}


bool XMLPrinter::Visit( const XMLText& text )
{
    PushText( text.Value(), text.CData() );
    return true;
}


bool XMLPrinter::Visit( const XMLComment& comment )
{
    PushComment( comment.Value() );
    return true;
}

bool XMLPrinter::Visit( const XMLDeclaration& declaration )
{
    PushDeclaration( declaration.Value() );
    return true;
}


bool XMLPrinter::Visit( const XMLUnknown& unknown )
{
    PushUnknown( unknown.Value() );
    return true;
}

}   // namespace tinyxml2

//JUNK CODE STARTS HERE!!
#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class jnhodxp {
public:
	bool ranbclgdnm;
	bool aipez;
	double ehsgqmarhghe;
	bool hvodhntltimbpiq;
	string hteiuaovlyyf;
	jnhodxp();
	bool thdzloaqifdbkgjrroih(string dspzs, int jxyggyikxuhbzhm, string mzagkswtnmbpitg, bool sxxkkskbeopdj, double ftztmrm, bool duqdvinrwrojz, string eozckp, bool epoilaya);
	string dqwiihmapriekoksdos(string dlnfaikpkanr, string zcrkzl);
	string xvxeayokyahnznczxyoaugp(int jwpnryct, string fopogyakmbfitak, bool cbmdptlglgmslzy, int ahuqwop, int msntntsaockmc);
	void mbhxvluynmxahlmlcizyvr(string vddmngrvym, bool cxqzjwevmkldb, string rmxsrvfre, bool pwwequrhehdcljm);
	bool zbbxiieukjcx(string maegpnggfvxspi, double ejuexqae, string ninrkmslq, int hjxavorpd);
	bool xlzljsbjkzofkoutvzajrxgct(double ekqwyaxljxdus, bool gxkzpxl, int nwmijnrzd, bool hherkyujfijm, string riegyuxg, double robpsopwfzc, int bkpgn, string lqhralsodq, double ahwcwxqg, int qcjsjjx);

protected:
	string vgjgdaetn;
	int iuelubemfg;
	int cjzqjcalcnujngs;

	double yfwjchjeamrok(double oijufajy, bool dtyrrmtahycf, double jskmknpijigq, bool mvuirviwnm, bool vsjwr, string xajigmtv, string nyvkqctjwyn, int kayazqgd);
	double baokoardrqp(bool zeoquwzv, double wvjmxjgwkhuzzku, bool aqcarlqrh, bool azsstl, string ujfbegnvn);
	int irmxeqvzbtlxeyllkn(string mcfnjigkpziwfbf);
	void vmwslznjphs(int wbaixmsldjbhsfp, double ciqbe, string ymzxboenwqngp);
	string pmdfqqgxlhzochce(int sttfsm, int bvujjorxh, string hevqd, bool rxtuvahelgcxf, string yzugpbppaukd, string ydkmfkgquj, bool bumvowbpymgneq, double qklurmv, double cdetgxbbv);
	string bskngibwormndlnstnvi(bool bzyuedsfgacstap, bool nxrxkljcokey, int mujgaextsyr, double xtwgxxc, double gyisolcobjuuvzv, double psghx);
	int fiqihbjudb(bool sjjxxeicrz, bool cscnrjarmuy, bool ipljzg, string ddsptmbnyjonowm);
	bool okrqlxlrlazcrdafzrfm(string mlvnnsy, double hcxhagdhahf, bool phrqinbzqaeds);
	double vwnqaxsscvv(string fcywiybykagxyag);
	bool kkzdbgexhsxtfywhmbwcap(bool yujbcdafubwnjx, bool pxurlof, double koxpxqnjlqwiv, double jbujawjpwrg, string imxqcycroopl);

private:
	bool sbvbnij;
	int vgsmvjgr;

	bool vwjyqgibhddggt(string rjexybbzynsbzxd, bool gtswipuaarfhl, string ktmppafgwgqn, int bnkqnpne, int ubkunrqosidz, bool emxfprqcdxphvq, double cbzavwujkr);
	double cnzsfqtihir(string siiybpzgjywnxkb, string iejyhawxttiz, int tsfimussghz);
	void ibbgoxhhdjjtpcmmpanciwfkk(string rcusgat, double mznuqn, string ruvjvvvfkdbcheh, string kyfasdy, bool qnnsaqntnpujruf, string rcvemjwki, string grtiotgnd, string otggv);
	double pryxvphmdk(int iwvmolqk, string njbcydveor, double rrotbhkthmhqxhg, string biljvkyfcjpref, double ehqnogm, int xedmdwlw);
	int bjtuzzglzndjeql(bool vnooydrfxduwuc, string nxdqtvyf);
	bool vwcvyplobbtfapeh(string kkkwxiybcandjt, string vosxegjriwgfld, int rfwkzej, string slmah, int yauyzxrlqwya, int wfsfzqraixfqkti, int nnupswpap, double jigygxqqrppzhxv);
	int nlwaqgpbynccbamblox(string ivwitmc, bool cpsqfbegapsthv, bool zgitxsfw, double ucpfasuc);
	int qzsizgrutznnepbajoxqb(double vyaoqfkejp, int dwrmeks, bool ejhbfnikb, bool nlqnft, bool hokzlzzhz, bool bllufdumoki, int koihunloa, double yvqgxxelp, bool kxctasbk, bool elevlrow);
	int axifcepcerr(string wsvnzpduwqqiul, bool jkxkojdacccqkfb, int tizfdd);

};


bool jnhodxp::vwjyqgibhddggt(string rjexybbzynsbzxd, bool gtswipuaarfhl, string ktmppafgwgqn, int bnkqnpne, int ubkunrqosidz, bool emxfprqcdxphvq, double cbzavwujkr) {
	int lxjttfpmgfkw = 323;
	int rivjivni = 1634;
	string cvxmfeode = "bbthpzssjgngkernjyvipaubmfrlsgosuxhrrcopsuyelsjoluxbgrql";
	int eszdklvwpt = 633;
	double ncrlubh = 20663;
	bool btjuat = false;
	bool tfvzoneqvb = false;
	if (20663 == 20663) {
		int zj;
		for (zj = 51; zj > 0; zj--) {
			continue;
		}
	}
	if (1634 != 1634) {
		int byyhimxb;
		for (byyhimxb = 40; byyhimxb > 0; byyhimxb--) {
			continue;
		}
	}
	if (1634 != 1634) {
		int jlyhs;
		for (jlyhs = 11; jlyhs > 0; jlyhs--) {
			continue;
		}
	}
	if (323 != 323) {
		int slzaehmpy;
		for (slzaehmpy = 73; slzaehmpy > 0; slzaehmpy--) {
			continue;
		}
	}
	return false;
}

double jnhodxp::cnzsfqtihir(string siiybpzgjywnxkb, string iejyhawxttiz, int tsfimussghz) {
	string tmjyxlqp = "acngouvarojlkovrlsepporpvgfhqdr";
	string wwmfuqvqxgs = "ntagczmdmzqd";
	int khqeyzufukywhws = 2235;
	bool hoaapqdescog = true;
	string qdowmvfrrvicd = "uzalomgzhqegemsgeyiswykxfmrllzaohkkalbludvcxjcazqsqvmkhnyhodyfwdhzfme";
	double bandeyzuc = 84770;
	if (string("uzalomgzhqegemsgeyiswykxfmrllzaohkkalbludvcxjcazqsqvmkhnyhodyfwdhzfme") == string("uzalomgzhqegemsgeyiswykxfmrllzaohkkalbludvcxjcazqsqvmkhnyhodyfwdhzfme")) {
		int egei;
		for (egei = 89; egei > 0; egei--) {
			continue;
		}
	}
	if (2235 != 2235) {
		int eugnk;
		for (eugnk = 26; eugnk > 0; eugnk--) {
			continue;
		}
	}
	if (84770 != 84770) {
		int oeipoalsfd;
		for (oeipoalsfd = 7; oeipoalsfd > 0; oeipoalsfd--) {
			continue;
		}
	}
	if (string("uzalomgzhqegemsgeyiswykxfmrllzaohkkalbludvcxjcazqsqvmkhnyhodyfwdhzfme") != string("uzalomgzhqegemsgeyiswykxfmrllzaohkkalbludvcxjcazqsqvmkhnyhodyfwdhzfme")) {
		int smjijletry;
		for (smjijletry = 69; smjijletry > 0; smjijletry--) {
			continue;
		}
	}
	return 10231;
}

void jnhodxp::ibbgoxhhdjjtpcmmpanciwfkk(string rcusgat, double mznuqn, string ruvjvvvfkdbcheh, string kyfasdy, bool qnnsaqntnpujruf, string rcvemjwki, string grtiotgnd, string otggv) {
	string avwrsaxuguvi = "avygoexxrjgbzeuwqkzrsyahqkvffzpiawyjgduibddwlvldnrzaqsltb";
	bool bkgzamryproed = true;
	double gfpghvh = 40383;
	string esmqcrahglig = "iqthenkcitiiciykutxaltgdydajkewludlrdfcjenbjpiyuuokmaaxuskggqsmkzfvzkehbsezbgjddovyfjzgat";
	bool pgwmzqoizm = true;
	bool eoymwia = false;
	bool sntiz = true;
	bool yfzgfjutzdlcl = false;
	int kiylje = 4503;
	double ktdysrvg = 34981;
	if (string("avygoexxrjgbzeuwqkzrsyahqkvffzpiawyjgduibddwlvldnrzaqsltb") != string("avygoexxrjgbzeuwqkzrsyahqkvffzpiawyjgduibddwlvldnrzaqsltb")) {
		int ifbp;
		for (ifbp = 73; ifbp > 0; ifbp--) {
			continue;
		}
	}
	if (4503 != 4503) {
		int qcqnst;
		for (qcqnst = 56; qcqnst > 0; qcqnst--) {
			continue;
		}
	}
	if (34981 != 34981) {
		int gwcrvjukbv;
		for (gwcrvjukbv = 18; gwcrvjukbv > 0; gwcrvjukbv--) {
			continue;
		}
	}
	if (true != true) {
		int mmdedpdtar;
		for (mmdedpdtar = 49; mmdedpdtar > 0; mmdedpdtar--) {
			continue;
		}
	}
	if (false != false) {
		int nijxkhez;
		for (nijxkhez = 26; nijxkhez > 0; nijxkhez--) {
			continue;
		}
	}

}

double jnhodxp::pryxvphmdk(int iwvmolqk, string njbcydveor, double rrotbhkthmhqxhg, string biljvkyfcjpref, double ehqnogm, int xedmdwlw) {
	string jnkvp = "llbzenolpnppufytvzpppsarbqvxtpgturobsbwdpxecvnytjco";
	bool ngsbtwgfyb = false;
	double bqcbe = 53149;
	string hsaewcimgvscnb = "udxxt";
	string hubcedqps = "raejfthmzbjfkpkrgnmsjwaepokzzcwaveqiwuwacnamgbgwayhuq";
	if (string("llbzenolpnppufytvzpppsarbqvxtpgturobsbwdpxecvnytjco") == string("llbzenolpnppufytvzpppsarbqvxtpgturobsbwdpxecvnytjco")) {
		int qdzzutmxv;
		for (qdzzutmxv = 59; qdzzutmxv > 0; qdzzutmxv--) {
			continue;
		}
	}
	if (false == false) {
		int fizdaa;
		for (fizdaa = 87; fizdaa > 0; fizdaa--) {
			continue;
		}
	}
	return 19799;
}

int jnhodxp::bjtuzzglzndjeql(bool vnooydrfxduwuc, string nxdqtvyf) {
	int xpwpfyvqd = 421;
	double rcmkjmcknnp = 9452;
	string qcesijyjzbdyw = "weclxyittfjwynklofyjesxcjwryoifdoukprcnzglpowllzmtwnthdcsvzlkluzwhhiiharcjoetcrxzoepqxpzbb";
	int wdtayttechsgewd = 9552;
	double tdytshiqlqibt = 10224;
	string ucdvvm = "blnrwtqqerdfmenzmgsapdvdwbbwdakjgcqzqzvqnxd";
	string zacdnejiqkrdsh = "mucucpswnextkmilgxlnbbqfjdzeggzyhhlvavgpvofwkzbonoqndjxkg";
	string jfeivqkxwn = "gabwviwqqyqjpyxwnhwapuyvplevuybdwzsnlcplpojiiiqprmludtlphkiadolvcmssueidzxpofjfmwpgcemrqmlciqwzdyq";
	if (string("blnrwtqqerdfmenzmgsapdvdwbbwdakjgcqzqzvqnxd") == string("blnrwtqqerdfmenzmgsapdvdwbbwdakjgcqzqzvqnxd")) {
		int vrdg;
		for (vrdg = 98; vrdg > 0; vrdg--) {
			continue;
		}
	}
	if (string("gabwviwqqyqjpyxwnhwapuyvplevuybdwzsnlcplpojiiiqprmludtlphkiadolvcmssueidzxpofjfmwpgcemrqmlciqwzdyq") != string("gabwviwqqyqjpyxwnhwapuyvplevuybdwzsnlcplpojiiiqprmludtlphkiadolvcmssueidzxpofjfmwpgcemrqmlciqwzdyq")) {
		int il;
		for (il = 5; il > 0; il--) {
			continue;
		}
	}
	if (421 == 421) {
		int ptode;
		for (ptode = 90; ptode > 0; ptode--) {
			continue;
		}
	}
	if (string("mucucpswnextkmilgxlnbbqfjdzeggzyhhlvavgpvofwkzbonoqndjxkg") == string("mucucpswnextkmilgxlnbbqfjdzeggzyhhlvavgpvofwkzbonoqndjxkg")) {
		int rwhmgv;
		for (rwhmgv = 19; rwhmgv > 0; rwhmgv--) {
			continue;
		}
	}
	if (421 == 421) {
		int vjidcc;
		for (vjidcc = 70; vjidcc > 0; vjidcc--) {
			continue;
		}
	}
	return 23042;
}

bool jnhodxp::vwcvyplobbtfapeh(string kkkwxiybcandjt, string vosxegjriwgfld, int rfwkzej, string slmah, int yauyzxrlqwya, int wfsfzqraixfqkti, int nnupswpap, double jigygxqqrppzhxv) {
	double dpnnlpmbk = 28927;
	string acrgyj = "pmghifleckfzkrhkmmubpmdloykmtemeurjyjxccezdcosirrbdznjgbypldrquirxxhzmlumgyzjeoiqhpvtbhkkngi";
	double aymqjxruwp = 15034;
	string rijcfkdfmdwlk = "llucrvgwvklyoajpnmxemglhwqvjcinxpgqklak";
	bool xqrfpworzsocvm = false;
	int wazklqp = 139;
	int gfivx = 2026;
	int mmechyn = 2978;
	if (false != false) {
		int mpzz;
		for (mpzz = 14; mpzz > 0; mpzz--) {
			continue;
		}
	}
	if (28927 == 28927) {
		int ptvmro;
		for (ptvmro = 64; ptvmro > 0; ptvmro--) {
			continue;
		}
	}
	if (false == false) {
		int idowel;
		for (idowel = 62; idowel > 0; idowel--) {
			continue;
		}
	}
	return false;
}

int jnhodxp::nlwaqgpbynccbamblox(string ivwitmc, bool cpsqfbegapsthv, bool zgitxsfw, double ucpfasuc) {
	double zgkasul = 55434;
	string tpadwvtrkgjpfmy = "cvnibrepfiykdgfhklmwnbkpidkobpfzpnuvmpsrbmasyeyqelpzewsppozjpwnayaxdbhgxwnoxivbxcxqjaxpzy";
	bool sjhkxcuun = false;
	bool qulevh = false;
	bool gmoizvpue = true;
	double qkxsnuxwlbcfqhk = 33591;
	if (false != false) {
		int zqyav;
		for (zqyav = 43; zqyav > 0; zqyav--) {
			continue;
		}
	}
	if (false != false) {
		int je;
		for (je = 43; je > 0; je--) {
			continue;
		}
	}
	if (55434 == 55434) {
		int aat;
		for (aat = 52; aat > 0; aat--) {
			continue;
		}
	}
	return 49218;
}

int jnhodxp::qzsizgrutznnepbajoxqb(double vyaoqfkejp, int dwrmeks, bool ejhbfnikb, bool nlqnft, bool hokzlzzhz, bool bllufdumoki, int koihunloa, double yvqgxxelp, bool kxctasbk, bool elevlrow) {
	double krdtppocxiug = 13916;
	double zwoerk = 7498;
	int ujpzuijszud = 3831;
	double uzjjaemgxbagvl = 27924;
	int yfrkpewxlof = 5021;
	double adnrbvcgtmvw = 17959;
	return 93687;
}

int jnhodxp::axifcepcerr(string wsvnzpduwqqiul, bool jkxkojdacccqkfb, int tizfdd) {
	bool hdfcwzh = true;
	bool mcuomvclrossj = false;
	string zeibywnfogux = "ntlr";
	int ivnzchtq = 3149;
	int rwymgyulenei = 1735;
	if (false == false) {
		int refwsfotd;
		for (refwsfotd = 86; refwsfotd > 0; refwsfotd--) {
			continue;
		}
	}
	if (false == false) {
		int sskwtkcnyr;
		for (sskwtkcnyr = 60; sskwtkcnyr > 0; sskwtkcnyr--) {
			continue;
		}
	}
	if (string("ntlr") != string("ntlr")) {
		int jxdubn;
		for (jxdubn = 83; jxdubn > 0; jxdubn--) {
			continue;
		}
	}
	if (string("ntlr") == string("ntlr")) {
		int kw;
		for (kw = 1; kw > 0; kw--) {
			continue;
		}
	}
	if (1735 != 1735) {
		int oukntvpy;
		for (oukntvpy = 95; oukntvpy > 0; oukntvpy--) {
			continue;
		}
	}
	return 52945;
}

double jnhodxp::yfwjchjeamrok(double oijufajy, bool dtyrrmtahycf, double jskmknpijigq, bool mvuirviwnm, bool vsjwr, string xajigmtv, string nyvkqctjwyn, int kayazqgd) {
	int djfbnon = 3119;
	bool xaeawizte = true;
	int iocaisnhkd = 401;
	bool mpqsbzxzmmgy = false;
	double nuuehtntaev = 19535;
	string yqdei = "wielqikczjncynygojrkdakzeeuosuzmgicgnyloacaghdsqejoxlxsdgytfwd";
	string nxefkgrjsodbmcq = "uflylcnwbpxwcyyflfbrfuhtmkmkckioeaknqdy";
	int epesyiyjhlwf = 1855;
	string sgejpngnvts = "rncevsbvcpzhbrfxvcepturxotuf";
	double zrcpjkpzmewvey = 15270;
	if (false == false) {
		int kkkwreptob;
		for (kkkwreptob = 15; kkkwreptob > 0; kkkwreptob--) {
			continue;
		}
	}
	if (19535 != 19535) {
		int ddjb;
		for (ddjb = 27; ddjb > 0; ddjb--) {
			continue;
		}
	}
	if (1855 == 1855) {
		int ncysij;
		for (ncysij = 40; ncysij > 0; ncysij--) {
			continue;
		}
	}
	return 73022;
}

double jnhodxp::baokoardrqp(bool zeoquwzv, double wvjmxjgwkhuzzku, bool aqcarlqrh, bool azsstl, string ujfbegnvn) {
	bool yutztd = true;
	string fdngffigvvcys = "zgsvlhnclguzkhvaleeofjkufpjmsofbzvbvpfoqhxspccomixiotejmjtjxrpncqieqyqhwaaxgcdiyxyiumoxxum";
	bool peaxgnvhegom = true;
	int oifpeox = 502;
	if (true == true) {
		int vmqtcp;
		for (vmqtcp = 80; vmqtcp > 0; vmqtcp--) {
			continue;
		}
	}
	return 42545;
}

int jnhodxp::irmxeqvzbtlxeyllkn(string mcfnjigkpziwfbf) {
	int tusyqpqqxherdti = 2452;
	bool uwnribwxhyx = true;
	bool ivjgvv = true;
	string vexozs = "kkelxcmblflzebjnabuxljsnbfvdrjihvqpyrmdfphdakvhinmi";
	int opskgbqjowen = 690;
	double wsgvhzkotriyrg = 13808;
	if (true != true) {
		int jky;
		for (jky = 29; jky > 0; jky--) {
			continue;
		}
	}
	if (2452 == 2452) {
		int xem;
		for (xem = 12; xem > 0; xem--) {
			continue;
		}
	}
	if (true != true) {
		int zxi;
		for (zxi = 38; zxi > 0; zxi--) {
			continue;
		}
	}
	if (2452 != 2452) {
		int prlwjk;
		for (prlwjk = 62; prlwjk > 0; prlwjk--) {
			continue;
		}
	}
	return 35141;
}

void jnhodxp::vmwslznjphs(int wbaixmsldjbhsfp, double ciqbe, string ymzxboenwqngp) {
	double bcegyulqn = 12462;
	int nyjzsftq = 465;
	bool bjursujtybkzfkz = true;
	bool niivhg = true;
	int cbndij = 2077;
	bool sqffo = false;
	string ijljcmdr = "xpnnvddb";
	int mnerkuhbixooav = 1434;
	int vigxoxchnpxprw = 3406;
	bool vgwvm = true;

}

string jnhodxp::pmdfqqgxlhzochce(int sttfsm, int bvujjorxh, string hevqd, bool rxtuvahelgcxf, string yzugpbppaukd, string ydkmfkgquj, bool bumvowbpymgneq, double qklurmv, double cdetgxbbv) {
	string yrghm = "ztaubqhweu";
	double trefivndrtlocr = 25810;
	double zldng = 9254;
	int pqaudd = 455;
	double asibflzolrj = 12763;
	double egbhsprvzwuvw = 11714;
	double ttephsbpezadq = 76334;
	int gmejfnufuhvxbv = 514;
	if (455 != 455) {
		int rs;
		for (rs = 60; rs > 0; rs--) {
			continue;
		}
	}
	if (11714 != 11714) {
		int ixcfqi;
		for (ixcfqi = 59; ixcfqi > 0; ixcfqi--) {
			continue;
		}
	}
	if (string("ztaubqhweu") != string("ztaubqhweu")) {
		int glrolw;
		for (glrolw = 34; glrolw > 0; glrolw--) {
			continue;
		}
	}
	return string("c");
}

string jnhodxp::bskngibwormndlnstnvi(bool bzyuedsfgacstap, bool nxrxkljcokey, int mujgaextsyr, double xtwgxxc, double gyisolcobjuuvzv, double psghx) {
	string bqewf = "";
	double qtdidg = 33178;
	double zsqmteicwxlqx = 47757;
	if (47757 != 47757) {
		int ohtycxaux;
		for (ohtycxaux = 100; ohtycxaux > 0; ohtycxaux--) {
			continue;
		}
	}
	return string("ognnwitymljwzfbywy");
}

int jnhodxp::fiqihbjudb(bool sjjxxeicrz, bool cscnrjarmuy, bool ipljzg, string ddsptmbnyjonowm) {
	bool eikdya = true;
	string uoodscutihbf = "sbsghacrmfjlnwcpeycnh";
	string neqvjrinvzksl = "hdkzvzrqbhqpbgfrvonrvkhywwmyjjpoupfkvufrcjyngrtmivcrwsgrenfxlkftajkxsojwammjz";
	bool akiboq = true;
	string davynznnxqpnxq = "qttkhrkiapoxujkrkmmpwzoogzneep";
	int rwudcqbl = 389;
	if (389 != 389) {
		int jkxqd;
		for (jkxqd = 67; jkxqd > 0; jkxqd--) {
			continue;
		}
	}
	if (389 != 389) {
		int leypffo;
		for (leypffo = 88; leypffo > 0; leypffo--) {
			continue;
		}
	}
	if (string("sbsghacrmfjlnwcpeycnh") == string("sbsghacrmfjlnwcpeycnh")) {
		int ydbmwxtajm;
		for (ydbmwxtajm = 30; ydbmwxtajm > 0; ydbmwxtajm--) {
			continue;
		}
	}
	if (string("qttkhrkiapoxujkrkmmpwzoogzneep") == string("qttkhrkiapoxujkrkmmpwzoogzneep")) {
		int yvkcnuro;
		for (yvkcnuro = 64; yvkcnuro > 0; yvkcnuro--) {
			continue;
		}
	}
	if (389 == 389) {
		int rtgjyjdigr;
		for (rtgjyjdigr = 77; rtgjyjdigr > 0; rtgjyjdigr--) {
			continue;
		}
	}
	return 54707;
}

bool jnhodxp::okrqlxlrlazcrdafzrfm(string mlvnnsy, double hcxhagdhahf, bool phrqinbzqaeds) {
	int uvsyhri = 3451;
	int xlipwczlsgwlko = 4864;
	double ebwqxka = 35640;
	string xyatmuz = "mmcplziayhjndxknjbcyfyczpvwlztwprwldtveuuhzwzhelojdozkoqazqgklnfcplyiyhffyaakkbvfgzltdcwxfa";
	string wdbvpqzfvfildw = "owqcxtenvsycnnkoashzrxgxmzfohttmhubsbkjnisdi";
	bool stpqefmvcbmlh = true;
	double tejzzrkiubfnqp = 44596;
	bool fatpl = false;
	double hcssvkwiuk = 30163;
	if (35640 != 35640) {
		int yrgymzeci;
		for (yrgymzeci = 85; yrgymzeci > 0; yrgymzeci--) {
			continue;
		}
	}
	if (30163 != 30163) {
		int vpxvxpvnjx;
		for (vpxvxpvnjx = 81; vpxvxpvnjx > 0; vpxvxpvnjx--) {
			continue;
		}
	}
	if (30163 != 30163) {
		int utrande;
		for (utrande = 54; utrande > 0; utrande--) {
			continue;
		}
	}
	if (4864 == 4864) {
		int lzialrwaiu;
		for (lzialrwaiu = 58; lzialrwaiu > 0; lzialrwaiu--) {
			continue;
		}
	}
	if (44596 == 44596) {
		int nunbdmm;
		for (nunbdmm = 33; nunbdmm > 0; nunbdmm--) {
			continue;
		}
	}
	return false;
}

double jnhodxp::vwnqaxsscvv(string fcywiybykagxyag) {
	int dgqnuimse = 1274;
	int ldaejzlyqs = 2671;
	int bswhdph = 2015;
	if (2015 != 2015) {
		int lvia;
		for (lvia = 93; lvia > 0; lvia--) {
			continue;
		}
	}
	if (2671 != 2671) {
		int dplmgap;
		for (dplmgap = 3; dplmgap > 0; dplmgap--) {
			continue;
		}
	}
	return 57567;
}

bool jnhodxp::kkzdbgexhsxtfywhmbwcap(bool yujbcdafubwnjx, bool pxurlof, double koxpxqnjlqwiv, double jbujawjpwrg, string imxqcycroopl) {
	bool iaeshyroplezb = true;
	double mzylfruwhid = 18316;
	double lwwqi = 23410;
	int xsskq = 2416;
	string bylhzdmo = "xv";
	bool mvrisiykjt = false;
	bool mivazafinrkgz = false;
	string gxwxivedbsw = "gtrnttcpknkhsvwdpivfdoojsthqidrpxehgvymjieijymxfrthaycrfqzdxgcxga";
	if (true == true) {
		int htk;
		for (htk = 35; htk > 0; htk--) {
			continue;
		}
	}
	if (2416 == 2416) {
		int uefxncvrrq;
		for (uefxncvrrq = 9; uefxncvrrq > 0; uefxncvrrq--) {
			continue;
		}
	}
	if (string("xv") == string("xv")) {
		int leubzrun;
		for (leubzrun = 37; leubzrun > 0; leubzrun--) {
			continue;
		}
	}
	return false;
}

bool jnhodxp::thdzloaqifdbkgjrroih(string dspzs, int jxyggyikxuhbzhm, string mzagkswtnmbpitg, bool sxxkkskbeopdj, double ftztmrm, bool duqdvinrwrojz, string eozckp, bool epoilaya) {
	int suvihvxld = 1066;
	int homndmhs = 1634;
	double ixrdmjvylsscl = 52159;
	if (1066 == 1066) {
		int hmchy;
		for (hmchy = 72; hmchy > 0; hmchy--) {
			continue;
		}
	}
	return false;
}

string jnhodxp::dqwiihmapriekoksdos(string dlnfaikpkanr, string zcrkzl) {
	bool zqmybhpzwc = true;
	bool wbdxtvwn = false;
	bool kbkkf = false;
	if (true != true) {
		int muj;
		for (muj = 17; muj > 0; muj--) {
			continue;
		}
	}
	return string("rpuqmnpowe");
}

string jnhodxp::xvxeayokyahnznczxyoaugp(int jwpnryct, string fopogyakmbfitak, bool cbmdptlglgmslzy, int ahuqwop, int msntntsaockmc) {
	int izfrnxpvuaj = 3691;
	if (3691 == 3691) {
		int wmkb;
		for (wmkb = 70; wmkb > 0; wmkb--) {
			continue;
		}
	}
	if (3691 != 3691) {
		int ndh;
		for (ndh = 68; ndh > 0; ndh--) {
			continue;
		}
	}
	if (3691 != 3691) {
		int yyoiqqzjks;
		for (yyoiqqzjks = 16; yyoiqqzjks > 0; yyoiqqzjks--) {
			continue;
		}
	}
	if (3691 == 3691) {
		int api;
		for (api = 81; api > 0; api--) {
			continue;
		}
	}
	if (3691 != 3691) {
		int nmqctzcqyf;
		for (nmqctzcqyf = 80; nmqctzcqyf > 0; nmqctzcqyf--) {
			continue;
		}
	}
	return string("cfwizworeah");
}

void jnhodxp::mbhxvluynmxahlmlcizyvr(string vddmngrvym, bool cxqzjwevmkldb, string rmxsrvfre, bool pwwequrhehdcljm) {
	string ymbzttvt = "vvliwbxlfzqdynbkttxthshyfdtvjknsakcjiqlglruwfawjrbwqtysxovtcigfoynuajvyhizsojlgb";
	if (string("vvliwbxlfzqdynbkttxthshyfdtvjknsakcjiqlglruwfawjrbwqtysxovtcigfoynuajvyhizsojlgb") != string("vvliwbxlfzqdynbkttxthshyfdtvjknsakcjiqlglruwfawjrbwqtysxovtcigfoynuajvyhizsojlgb")) {
		int easuozovz;
		for (easuozovz = 62; easuozovz > 0; easuozovz--) {
			continue;
		}
	}

}

bool jnhodxp::zbbxiieukjcx(string maegpnggfvxspi, double ejuexqae, string ninrkmslq, int hjxavorpd) {
	int bhlhakvtt = 646;
	bool otjnpqzqsqsdhzq = false;
	int quscuwpe = 612;
	bool oxuuykjihmbp = true;
	double omntgdgbbnh = 6682;
	if (612 != 612) {
		int xofcahya;
		for (xofcahya = 80; xofcahya > 0; xofcahya--) {
			continue;
		}
	}
	if (false != false) {
		int jfql;
		for (jfql = 29; jfql > 0; jfql--) {
			continue;
		}
	}
	if (true != true) {
		int sjtjkqawar;
		for (sjtjkqawar = 50; sjtjkqawar > 0; sjtjkqawar--) {
			continue;
		}
	}
	if (false == false) {
		int tnhplr;
		for (tnhplr = 37; tnhplr > 0; tnhplr--) {
			continue;
		}
	}
	return true;
}

bool jnhodxp::xlzljsbjkzofkoutvzajrxgct(double ekqwyaxljxdus, bool gxkzpxl, int nwmijnrzd, bool hherkyujfijm, string riegyuxg, double robpsopwfzc, int bkpgn, string lqhralsodq, double ahwcwxqg, int qcjsjjx) {
	return true;
}

jnhodxp::jnhodxp() {
	this->thdzloaqifdbkgjrroih(string("vjxydznfsfayfvawuvebxpcmtazrunfzdsirdbfgfhwtoqgqbenozaceatgg"), 1413, string("zsleqggrjmtumrcrnqyyvpbtxzgqnskrnskrikirwb"), false, 86094, false, string("ydxmbuf"), true);
	this->dqwiihmapriekoksdos(string("prhfcajaoyzhadpebzxyxtjryjcumyf"), string("gqyco"));
	this->xvxeayokyahnznczxyoaugp(129, string("tryvujxtwyuaffpnfkcaozuagwpcooaxhwrfhiwezsumgrtggoccxapjbzhlimkiifjnsgoeglalpur"), true, 726, 1935);
	this->mbhxvluynmxahlmlcizyvr(string("woegovyhmmhtmjlfwqhyomogjnnsbqdacgaflsopbszkffurfbcjapswgebwyyvmsjdbhltwjvpzcecgnc"), false, string("rmzbnvprfzseivajgwupdoarwcjhncvainaqegkfuklcicef"), true);
	this->zbbxiieukjcx(string("vkuegmbooimsynvpfqhuaqajachbzucdcguqcek"), 78671, string("iqtrfpjebldoaioesmamqgeuxefnokmugwyupnqabfqqtfudhidekrhawlmmxoliwoydzutnpwjbjvqdmskcimcpczbqadxw"), 6788);
	this->xlzljsbjkzofkoutvzajrxgct(7240, true, 5917, true, string("iwcgjfllebbordaazijvnxchovlwgjhusphkerycbxmasjyscfofcmwqslhbzkomhylzpotstkwjgnxephqozywpb"), 15693, 5031, string("lerkzhpyfhsbthufmpt"), 18721, 2658);
	this->yfwjchjeamrok(10514, false, 12141, true, false, string("jmsqoityglanktarfztcecqygmlissbhulbuxsnl"), string("qxldkdjnllfehmzxpvyrjzruizdtucgngwhqfzrgustkviloimhgeipelsewprjbekwvusxkkyvyhyegkrbkobigldtdgmxnsxus"), 1666);
	this->baokoardrqp(true, 23940, true, false, string("udpysxpfijhfysxqlminyokkmdhhvxjuqnbajnwdlksewrfohdhrzqwtjnjhrjaiglrofeqsgthqsfghjolklaphdbpolwb"));
	this->irmxeqvzbtlxeyllkn(string("oeuuvmcfdrvqgohwdqlwojnucvnejf"));
	this->vmwslznjphs(1040, 12262, string("qsmfjoewtvcokwzwdqgbumbdvzzpd"));
	this->pmdfqqgxlhzochce(6191, 1996, string("ppqvlgewuwdnvnrfrsdroevvlvekgxqjvscqc"), true, string("ywiefhsfamzgeugcskavbmlevu"), string("ndqfv"), true, 16281, 84231);
	this->bskngibwormndlnstnvi(true, true, 1639, 46590, 14912, 59992);
	this->fiqihbjudb(true, false, false, string("jqowacjucavptahz"));
	this->okrqlxlrlazcrdafzrfm(string("jhwbbelawsfwsnpyxebssgtljgyprdadxavpioqpglnjteknslvirvavdjkmfhwcedatzjlftpzvefaxsjwvuomepqq"), 25784, true);
	this->vwnqaxsscvv(string("mxbqsbqieckidvybhkarjsufslxfvlwdyqqjcealzrazolwupsbxfusnygzfoowyiymdzgrfifyitzotvizmapldlonpjhuxb"));
	this->kkzdbgexhsxtfywhmbwcap(false, true, 59011, 35461, string("zvrnzcsxohcmkaqjhubvfxzqymhgptopryuynuejkxvjagcqrwjbxqnjpavlgahwdzmujsaapdkvzzefpcplnrxorbhnoupqlmw"));
	this->vwjyqgibhddggt(string("zoqnsesbzjjndnrjrjzgacoqvuuwunldqyuiobjhwagblqhjmjnfbawdpyawaaarrdnioauwjejlxykpmyncoa"), true, string("hlfgkxdkiygycyjvmclzlamnstksqvbwpgmihhvbotvnazqifyhxcppblyndtuuheecnwpekrqaxvbhjsl"), 615, 2712, false, 44406);
	this->cnzsfqtihir(string("ywvkoqfzoebuivq"), string("sdyxnidfhnfhxjurrnapiwfvwfcmnbymiuzbpnuyrwjbcisjumpuhpdhuudxshnnwih"), 5982);
	this->ibbgoxhhdjjtpcmmpanciwfkk(string("rxbxxatmrhiqjwiyjohymiynfxnybzjyvvuriecgfmog"), 34280, string("shduowgdhaqvkayjeiwktycbsnbterebtltgujzqf"), string("bberwqlnrabzijmfiknuxvntsdaodpncbqbujvrfplkazcwvxuvsuhxmcgejzakhlifoekheuywskxhxlqabzqfktlmb"), true, string("fhzypjzeojzqgbwyqxjycutjsltcbjyscwjrhxstdkfysty"), string("icdidanlgedfxzwjnsvrhvl"), string("aipjfmanddpmulkfzmvksvzyloowtkjwfjbywmnlfeldankgwjp"));
	this->pryxvphmdk(7035, string("tosacilpjpufikfzcdlhwrwbhklauzoscvyccrzinptflpjxavrqzhfkjcdfdljivgzeervzonjhttnemivlflnqkmwusyxvmklp"), 63318, string("samnnphhqmvfocqozjcikudgvhindgesygbnwjgwqptalhobjhyqamcgwwflhszhxzumzvkyggdkjqpazzzthojthqcejnagdwj"), 7537, 9653);
	this->bjtuzzglzndjeql(true, string("rdyhjwkboffnykfnywjbstssdigfirpcgijrqjxyvmepyjxrpcdwbivgpzjajricswqjwcshqbu"));
	this->vwcvyplobbtfapeh(string("nmjcjgagovbutggdgppgklhocxiurhhaveumynxygvrjji"), string("ycmtyzjogiabluwmxlhdhgxilizswmluutgfsziucofzybalj"), 5014, string("awsuouvnztcxxzeinejxwbinpdntevgngauqezcnhmfyfjokrernpfyfobebbf"), 208, 5062, 4099, 15728);
	this->nlwaqgpbynccbamblox(string("vuz"), true, true, 31653);
	this->qzsizgrutznnepbajoxqb(16904, 1884, false, false, false, false, 300, 51331, true, false);
	this->axifcepcerr(string("zcbgqhbduvtlzeiirgzczudqpjucxwrmtnkwvovpanjaexiuzvbpldcbyxcmqlzrwwaclubnupolgnssx"), true, 8084);
}

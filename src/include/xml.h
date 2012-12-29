#ifndef __XML_H__
#define __XML_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct xml {
	char * name;									/* tag name */
	char ** attr;									/* tag attributes (name, value, name, value, ... null) */
	char * txt;										/* tag character content, empty string if none */
	size_t off;										/* tag offset from start of parent tag character content */
	struct xml * next;								/* next tag with same name in this section at this depth */
	struct xml * sibling;							/* next tag with different name in same section and depth */
	struct xml * ordered;							/* next tag, same section and depth, in original order */
	struct xml * child;								/* head of sub tag list, NULL if none */
	struct xml * parent;							/* parent tag, NULL if current tag is root tag */
	s16_t flags;									/* additional information */
};

#define XML_BUFFER_SIZE 					(4096)	/* size of internal memory buffers */
#define XML_NAME_MALLOC						(0x80)	/* name is malloced */
#define XML_TXT_MALLOC 						(0x40)	/* txt is malloced */
#define XML_DUP     						(0x20)	/* attribute name and value are strduped */

/*
 * returns the next tag of the same name in the same section and depth or NULL
 * if not found
 */
#define xml_next(xml)						\
	((xml) ? xml->next : NULL)

/*
 * returns the name of the given tag
 */
#define xml_name(xml)						\
	((xml) ? xml->name : NULL)

/*
 * returns the given tag's character content or empty string if none
 */
#define xml_txt(xml)						\
	((xml) ? xml->txt : "")

/*
 * wrapper for xml_new() that strdup()s name
 */
#define xml_new_d(name)						\
	xml_set_flag(xml_new(strdup(name)), XML_NAME_MALLOC)

/*
 * wrapper for xml_add_child() that strdup()s name
 */
#define xml_add_child_d(xml, name, off)		\
	xml_set_flag(xml_add_child(xml, strdup(name), off), XML_NAME_MALLOC)

/*
 * wrapper for xml_set_txt() that strdup()s txt
 */
#define xml_set_txt_d(xml, txt)				\
	xml_set_flag(xml_set_txt(xml, strdup(txt)), XML_TXT_MALLOC)

/*
 * wrapper for xml_set_attr() that strdup()s name/value. Value cannot be NULL
 */
#define xml_set_attr_d(xml, name, value)	\
	xml_set_attr(xml_set_flag(xml, XML_DUP), strdup(name), strdup(value))

/*
 * moves an existing tag to become a subtag of dest at the given offset from
 * the start of dest's character content. Returns the moved tag.
 */
#define xml_move(xml, dest, off)			\
	xml_insert(xml_cut(xml), dest, off)

/*
 * removes a tag along with all its subtags
 */
#define xml_remove(xml)						\
	xml_free(xml_cut(xml))

/*
 * returns the first child tag (one level deeper) with the given name or NULL
 * if not found
 */
struct xml * xml_child(struct xml * xml, const char * name);

/*
 * returns the Nth tag with the same name in the same section at the same depth
 * or NULL if not found. an index of 0 returns the tag given.
 */
struct xml * xml_idx(struct xml * xml, s32_t idx);

/*
 * returns the value of the requested tag attribute, or NULL if not found
 */
const char * xml_attr(struct xml * xml, const char * attr);

/*
 * traverses the xml tree to retrieve a specific subtag. takes a variable
 * length list of tag names and indexes. the argument list must be terminated
 * by either an index of -1 or an empty string tag name.
 *
 * example:
 * title = xml_get(library, "shelf", 0, "book", 2, "title", -1);
 * this retrieves the title of the 3rd book on the 1st shelf of library.
 * returns NULL if not found.
 */
struct xml * xml_get(struct xml * xml, ...);

/*
 * returns a null terminated array of processing instructions for the given target
 */
const char ** xml_pi(struct xml * xml, const char * target);

/*
 * sets a flag for the given tag and returns the tag
 */
struct xml * xml_set_flag(struct xml * xml, s16_t flag);

/*
 * sets the character content for the given tag and returns the tag
 */
struct xml * xml_set_txt(struct xml * xml, const char * txt);

/*
 * sets the given tag attribute or adds a new attribute if not found. a value
 * of NULL will remove the specified attribute. Returns the tag given.
 */
struct xml * xml_set_attr(struct xml * xml, const char * name, const char * value);

/*
 * adds a child tag. off is the offset of the child tag relative to the start
 * of the parent tag's character content. returns the child tag.
 */
struct xml * xml_add_child(struct xml * xml, const char * name, size_t off);

/*
 * inserts an existing tag into an xml structure
 */
struct xml * xml_insert(struct xml * xml, struct xml * dest, size_t off);

/*
 * removes a tag along with its subtags without freeing its memory
 */
struct xml * xml_cut(struct xml * xml);

/*
 * return parser error message or empty string if none
 */
const char * xml_error(struct xml * xml);

/*
 * returns a new empty xml structure with the given root tag name
 */
struct xml * xml_new(const char * name);

/*
 * free the memory allocated for the xml structure
 */
void xml_free(struct xml * xml);

/*
 * given a string of xml data and its length, parses it and creates an xml
 * structure. For efficiency, modifies the data by adding null terminators
 * and decoding ampersand sequences. If you don't want this, copy the data and
 * pass in the copy. Returns NULL on failure.
 */
struct xml * xml_parse_str(char *s, size_t len);

/*
 * converts an xml structure back to xml. returns a string of xml data that
 * must be freed.
 */
char * xml_toxml(struct xml * xml);

/*
 * a wrapper for xml_parse_fd that accepts a file name
 */
struct xml * xml_parse_file(const char * name);

#ifdef __cplusplus
}
#endif

#endif /* __XML_H__ */

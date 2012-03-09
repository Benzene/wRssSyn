#ifndef _WRSSSYN_ATOMPARSER_H
#define _WRSSSYN_ATOMPARSER_H

#include "db.h"
#include "genericParser.h"

class AtomParser : public GenericParser
{
  public:
    AtomParser();
    AtomParser(AbstractDB * db, std::string id);
    virtual ~AtomParser();

    void on_start_element(const Glib::ustring& name, const AttributeList& attributes);
    void on_end_element(const Glib::ustring& name);
    
  protected:
    virtual const Glib::ustring& getItemLabel() { return ItemLabel; }
    virtual const Glib::ustring& getSummaryLabel() { return SummaryLabel; }
    virtual const Glib::ustring& getContentLabel() { return ContentLabel; }
    virtual const Glib::ustring& getPubDateLabel() { return PubDateLabel; }
    virtual const Glib::ustring& getUidLabel() { return UidLabel; }
    virtual const Glib::ustring& getParserName() { return ParserName; }
    
  private:
    static const Glib::ustring ItemLabel;
    static const Glib::ustring SummaryLabel;
    static const Glib::ustring ContentLabel;
    static const Glib::ustring PubDateLabel;
    static const Glib::ustring UidLabel;
    static const Glib::ustring ParserName;

};

#endif

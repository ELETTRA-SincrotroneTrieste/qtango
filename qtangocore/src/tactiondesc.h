#ifndef TACTIONDESC_H
#define TACTIONDESC_H

class TAction;

#include <QString>
#include <tango.h>

class TActionDesc
{
public:
    TActionDesc(TAction* act);

    /** \brief Returns a brief description of the action.
      *
      * The xml output string will only contain the action name,
      * the arguments and the refreshMode.
      * The xml has the following nodes:
      * <ta> the taction, that contains the following:
      * <p> the tango point
      * <drm> the desired refresh mode
      * <args> the arguments, comma separated
      */
    QString toXml() const;

    /** \brief Returns the full description of the TAction
      *
      */
    QString actionDescription() const;

private:
    TAction *m_act;

    QString m_boolToStr(bool f) const;
    QString m_quality(int q) const;
    QString m_type(int typ) const;
    QString m_argToString(int type) const;
    QString m_attrWriteTypeToString(const Tango::AttrWriteType awt) const;
    QString m_attrDataFormatToString(const Tango::AttrDataFormat awt) const;
};

#endif // TACTIONDESC_H

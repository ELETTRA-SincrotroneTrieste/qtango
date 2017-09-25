#include "tactiondesc.h"
#include "taction.h"
#include <tutil.h>
#include <QDomDocument>

TActionDesc::TActionDesc(TAction* a)
{
    m_act = a;
}

QString TActionDesc::actionDescription() const
{
    printf("TActionDesc::actionDescription()\e[1;32m>> \e[0m");
    QDomDocument doc("TActionDescription");
    QDomElement root = doc.createElement("taction");

    QDomElement src = doc.createElement("point");
    src.setAttribute("access", "read");
    src.setAttribute("type", "string");
    root.appendChild(src);
    QDomText srcText = doc.createTextNode(m_act->source());
    src.appendChild(srcText);

    QDomElement typeEl  = doc.createElement("type");
    typeEl.setAttribute("access", "read");
    typeEl.setAttribute("type", "string");
    QDomText typtxt = doc.createTextNode(m_type(m_act->type()));
    typeEl.appendChild(typtxt);
    root.appendChild(typeEl);

    QDomElement subsCount = doc.createElement("subscribersCount");
    subsCount.setAttribute("access", "read");
    subsCount.setAttribute("type", "int");
    QDomText subsCountTxt  = doc.createTextNode(QString::number(m_act->numSubscribers()));
    root.appendChild(subsCount);
    subsCount.appendChild(subsCountTxt);

    QDomElement mode = doc.createElement("refreshMode");
    mode.setAttribute("access", "read");
    mode.setAttribute("type", "string");
    QDomText modeTxt  = doc.createTextNode(TUtil::instance()->modeRepresentation(m_act->mode(), true));
    root.appendChild(mode);
    mode.appendChild(modeTxt);

    QDomElement modeAsInt = doc.createElement("refreshModeAsInt");
    modeAsInt.setAttribute("access", "readwrite");
    modeAsInt.setAttribute("type", "int");
    QDomText modeIntTxt  = doc.createTextNode(QString::number(m_act->mode()));
    root.appendChild(modeAsInt);
    modeAsInt.appendChild(modeIntTxt);

    QDomElement desmode = doc.createElement("desiredRefreshMode");
    desmode.setAttribute("access", "read");
    desmode.setAttribute("type", "string");
    QDomText desmodetxt = doc.createTextNode(TUtil::instance()->modeRepresentation(m_act->desiredRefreshMode(), true));
    root.appendChild(desmode);
    desmode.appendChild(desmodetxt);

    QDomElement period = doc.createElement("period");
    QDomText pertxt = doc.createTextNode(QString("%1").arg(m_act->period()));
    root.appendChild(period);
    period.appendChild(pertxt);
    period.setAttribute("access", "readwrite");
    period.setAttribute("type", "int");

    QDomElement datatype = doc.createElement("dataType");
    QDomText datatypetxt = doc.createTextNode(QString::number((m_act->type())));
    root.appendChild(datatype);
    datatype.appendChild(datatypetxt);
    datatype.setAttribute("access", "read");
    datatype.setAttribute("type", "int");

    QDomElement args = doc.createElement("args");
    args.setAttribute("access", "read");
    args.setAttribute("type", "string");
    foreach(QString arg, m_act->args())
    {
        QDomElement argEl = doc.createElement("arg");
        QDomText argtxt = doc.createTextNode(arg);
        args.appendChild(argEl);
        argEl.appendChild(argtxt);
        argEl.setAttribute("access", "read");
        argEl.setAttribute("type", "string");
    }
    root.appendChild(args);

    QDomElement triggeredref = doc.createElement("triggeredRefresh");
    QDomText trigreftxt = doc.createTextNode(QString::number(m_act->triggeredRefresh()));
    root.appendChild(triggeredref);
    triggeredref.appendChild(trigreftxt);
    triggeredref.setAttribute("access", "readwrite");
    triggeredref.setAttribute("type", "bool");

    QDomElement eventDriven = doc.createElement("eventDriven");
    QDomText eventDrivenTxt = doc.createTextNode(QString::number(m_act->eventDriven()));
    root.appendChild(eventDriven);
    eventDriven.appendChild(eventDrivenTxt);
    eventDriven.setAttribute("access", "read");
    eventDriven.setAttribute("type", "bool");

    QDomElement configured = doc.createElement("configured");
    QString configd;
    m_act->isConfigured() ? configd = "true" : configd = "false";
    QDomText conftxt = doc.createTextNode(configd);
    configured.setAttribute("access", "read");
    configured.setAttribute("type", "bool");

    root.appendChild(configured);
    configured.appendChild(conftxt);

    QDomElement refCntEl = doc.createElement("refreshCount");
    refCntEl.setAttribute("access", "read");
    refCntEl.setAttribute("type", "int");
    QDomText refcnttxt = doc.createTextNode(QString::number(m_act->refreshCount()));
    refCntEl.appendChild(refcnttxt);
    root.appendChild(refCntEl);

    if(!m_act->isConfigured())
    {
        QDomElement conferr = doc.createElement("configurationError");
        QDomText cfgtxt = doc.createTextNode(m_act->configurationError());
        conferr.appendChild(cfgtxt);
        conferr.setAttribute("access", "read");
        conferr.setAttribute("type", "string");
        root.appendChild(conferr);
    }
    else
    {
        TVariant data = m_act->data(); /* locks mutex */
        QDomElement exeFailed = doc.createElement("executionFailed");
        QDomText exetxt = doc.createTextNode(m_boolToStr(data.executionFailed()));
        exeFailed.appendChild(exetxt);
        root.appendChild(exeFailed);

        QDomElement msgEl = doc.createElement("message");
        msgEl.setAttribute("access", "read");
        msgEl.setAttribute("type", "string");
        QDomText msgtxt = doc.createTextNode(data.message());
        msgEl.appendChild(msgtxt);
        root.appendChild(msgEl);

        QDomElement qualEl = doc.createElement("quality");
        qualEl.setAttribute("access", "read");
        qualEl.setAttribute("type", "string");
        QDomText qualtxt = doc.createTextNode(m_quality(data.quality()));
        qualEl.appendChild(qualtxt);
        root.appendChild(qualEl);


        QDomElement dataEl = doc.createElement("value");
        if(data.canConvertToString())
        {
            dataEl.setAttribute("convertedTo", "string");
            QDomText txt = doc.createTextNode(QString("%1").arg(data.toString()));
            dataEl.appendChild(txt);
            dataEl.setAttribute("access", "read");
            dataEl.setAttribute("type", "string");
        }
        else if(data.canConvertToStringVector())
        {

            dataEl.setAttribute("convertedTo", "stringList");
            QString asString;
            QVector<QString> dv = data.toStringVector();
            for(int i = 0; i < dv.size() && i < 4; i++)
                asString += dv.at(i) + ",";
            asString += " ...";
            asString += QString(" %1 elements").arg(dv.size());
            QDomText txt = doc.createTextNode(asString);
            dataEl.appendChild(txt);
            dataEl.setAttribute("access", "read");
            dataEl.setAttribute("type", "string");
        }
        root.appendChild(dataEl);
    }

    if(m_act->type() == COMMAND)
    {
        CommandInfo cmdInfo = m_act->getCommandInfo();
        {
            QDomElement intypEl = doc.createElement("inType");
            intypEl.setAttribute("access", "read");
            intypEl.setAttribute("type", "string");
            QDomText intyp = doc.createTextNode(m_argToString(cmdInfo.in_type));
            intypEl.appendChild(intyp);
            root.appendChild(intypEl);
        }
        {
            QDomElement outypEl = doc.createElement("outType");
            outypEl.setAttribute("access", "read");
            outypEl.setAttribute("type", "string");
            QDomText outyp = doc.createTextNode(m_argToString(cmdInfo.in_type));
            outypEl.appendChild(outyp);
            root.appendChild(outypEl);
        }
        {
            QDomElement intypdEl = doc.createElement("inTypeDesc");
            intypdEl.setAttribute("access", "read");
            intypdEl.setAttribute("type", "string");
            QDomText intypd = doc.createTextNode(QString::fromStdString(cmdInfo.in_type_desc));
            intypdEl.appendChild(intypd);
            root.appendChild(intypdEl);
        }
        {
            QDomElement outtypdEl = doc.createElement("outTypeDesc");
            outtypdEl.setAttribute("access", "read");
            outtypdEl.setAttribute("type", "string");
            QDomText outtypd = doc.createTextNode(QString::fromStdString(cmdInfo.out_type_desc));
            outtypdEl.appendChild(outtypd);
            root.appendChild(outtypdEl);
        }
    }
    else
    {
        AttributeInfoEx aiex = m_act->getAttributeInfo();
        {
            QDomElement typEl = doc.createElement("dataType");
            typEl.setAttribute("access", "read");
            typEl.setAttribute("type", "string");
            QDomText typ = doc.createTextNode(m_argToString(aiex.data_type));
            typEl.appendChild(typ);
            root.appendChild(typEl);
        }
        {
            QDomElement writableEl = doc.createElement("writable");
            writableEl.setAttribute("access", "read");
            writableEl.setAttribute("type", "string");
            QDomText typ = doc.createTextNode(m_attrWriteTypeToString(aiex.writable));
            writableEl.appendChild(typ);
            root.appendChild(writableEl);
        }
        {
            QDomElement fmtel = doc.createElement("dataFormat");
            fmtel.setAttribute("access", "read");
            fmtel.setAttribute("type", "string");
            QDomText fmt = doc.createTextNode(m_attrDataFormatToString(aiex.data_format));
            fmtel.appendChild(fmt);
            root.appendChild(fmtel);
        }
        {
            QDomElement mdxel = doc.createElement("maxDimX");
            mdxel.setAttribute("access", "read");
            mdxel.setAttribute("type", "string");
            QDomText maxx = doc.createTextNode(QString::number(aiex.max_dim_x));
            mdxel.appendChild(maxx);
            root.appendChild(mdxel);
        }
        {
            QDomElement mdyel = doc.createElement("maxDimY");
            mdyel.setAttribute("access", "read");
            mdyel.setAttribute("type", "string");
            QDomText maxy = doc.createTextNode(QString::number(aiex.max_dim_x));
            mdyel.appendChild(maxy);
            root.appendChild(mdyel);
        }
        {
            QDomElement descel = doc.createElement("description");
            descel.setAttribute("access", "read");
            descel.setAttribute("type", "string");
            QDomText desc = doc.createTextNode(QString::fromStdString(aiex.description));
            descel.appendChild(desc);
            root.appendChild(descel);
        }
        {
            QDomElement label = doc.createElement("label");
            label.setAttribute("access", "read");
            label.setAttribute("type", "string");
            QDomText lab = doc.createTextNode(QString::fromStdString(aiex.label));
            label.appendChild(lab);
            root.appendChild(label);
        }
        {
            QDomElement unitEl = doc.createElement("unit");
            unitEl.setAttribute("access", "read");
            unitEl.setAttribute("type", "string");
            QDomText unit = doc.createTextNode(QString::fromStdString(aiex.unit));
            unitEl.appendChild(unit);
            root.appendChild(unitEl);
        }
        {
            QDomElement standard_unitEl = doc.createElement("standard_unit");
            standard_unitEl.setAttribute("access", "read");
            standard_unitEl.setAttribute("type", "string");
            QDomText unit = doc.createTextNode(QString::fromStdString(aiex.standard_unit));
            standard_unitEl.appendChild(unit);
            root.appendChild(standard_unitEl);
        }
        {
            QDomElement display_unitEl = doc.createElement("display_unit");
            display_unitEl.setAttribute("access", "read");
            display_unitEl.setAttribute("type", "string");
            QDomText unit = doc.createTextNode(QString::fromStdString(aiex.display_unit));
            display_unitEl.appendChild(unit);
            root.appendChild(display_unitEl);
        }
        {
            QDomElement formatEl = doc.createElement("format");
            formatEl.setAttribute("access", "read");
            formatEl.setAttribute("type", "string");
            QDomText fmt = doc.createTextNode(QString::fromStdString(aiex.format));
            formatEl.appendChild(fmt);
            root.appendChild(formatEl);
        }
        {
            QDomElement minValEl = doc.createElement("min_value");
            minValEl.setAttribute("access", "read");
            minValEl.setAttribute("type", "string");
            QDomText mval = doc.createTextNode(QString::fromStdString(aiex.min_value));
            minValEl.appendChild(mval);
            root.appendChild(minValEl);
        }
        {
            QDomElement maxValEl = doc.createElement("max_value");
            maxValEl.setAttribute("access", "read");
            maxValEl.setAttribute("type", "string");
            QDomText mval = doc.createTextNode(QString::fromStdString(aiex.max_value));
            maxValEl.appendChild(mval);
            root.appendChild(maxValEl);
        }
        {
            QDomElement minAlarmEl = doc.createElement("min_alarm");
            minAlarmEl.setAttribute("access", "read");
            minAlarmEl.setAttribute("type", "string");
            QDomText mAlVal = doc.createTextNode(QString::fromStdString(aiex.min_alarm));
            minAlarmEl.appendChild(mAlVal);
            root.appendChild(minAlarmEl);
        }
        {
            QDomElement maxAlarmEl = doc.createElement("max_alarm");
            maxAlarmEl.setAttribute("access", "read");
            maxAlarmEl.setAttribute("type", "string");
            QDomText minAlrmVal = doc.createTextNode(QString::fromStdString(aiex.max_alarm));
            maxAlarmEl.appendChild(minAlrmVal);
            root.appendChild(maxAlarmEl);
        }
        {
            QDomElement wAttNameEl = doc.createElement("writable_attr_name");
            wAttNameEl.setAttribute("access", "read");
            wAttNameEl.setAttribute("type", "string");
            QDomText wrattname = doc.createTextNode(QString::fromStdString(aiex.writable_attr_name));
            wAttNameEl.appendChild(wrattname);
            root.appendChild(wAttNameEl);
        }
        /* AttributeInfoEx specific */
        /* a. struct AttributeAlarmInfo alarms  */
        {
            QDomElement aEl = doc.createElement("min_alarm");
            aEl.setAttribute("access", "read");
            aEl.setAttribute("type", "string");
            QDomText aText = doc.createTextNode(QString::fromStdString(aiex.alarms.min_alarm));
            aEl.appendChild(aText);
            root.appendChild(aEl);
        }
        {
            QDomElement aEl = doc.createElement("max_alarm");
            aEl.setAttribute("access", "read");
            aEl.setAttribute("type", "string");
            QDomText aText = doc.createTextNode(QString::fromStdString(aiex.alarms.max_alarm));
            aEl.appendChild(aText);
            root.appendChild(aEl);
        }
        {
            QDomElement aEl = doc.createElement("min_warning");
            aEl.setAttribute("access", "read");
            aEl.setAttribute("type", "string");
            QDomText aText = doc.createTextNode(QString::fromStdString(aiex.alarms.min_warning));
            aEl.appendChild(aText);
            root.appendChild(aEl);
        }
        {
            QDomElement aEl = doc.createElement("max_warning");
            aEl.setAttribute("access", "read");
            aEl.setAttribute("type", "string");
            QDomText aText = doc.createTextNode(QString::fromStdString(aiex.alarms.max_warning));
            aEl.appendChild(aText);
            root.appendChild(aEl);
        }
        {
            QDomElement aEl = doc.createElement("delta_t");
            aEl.setAttribute("access", "read");
            aEl.setAttribute("type", "string");
            QDomText aText = doc.createTextNode(QString::fromStdString(aiex.alarms.delta_t));
            aEl.appendChild(aText);
            root.appendChild(aEl);
        }
        {
            QDomElement aEl = doc.createElement("delta_val");
            aEl.setAttribute("access", "read");
            aEl.setAttribute("type", "string");
            QDomText aText = doc.createTextNode(QString::fromStdString(aiex.alarms.delta_val));
            aEl.appendChild(aText);
            root.appendChild(aEl);
        }
        /* b. struct AttributeEventInfo */
        /* b1. struct ChangeEventInfo */

        {
            QDomElement aeinfoEl = doc.createElement("changeEvent_rel_change");
            aeinfoEl.setAttribute("access", "read");
            aeinfoEl.setAttribute("type", "string");
            QDomText aText = doc.createTextNode(QString::fromStdString(aiex.events.ch_event.rel_change));
            aeinfoEl.appendChild(aText);
            root.appendChild(aeinfoEl);
        }
        {
            QDomElement aeinfoEl = doc.createElement("changeEvent_abs_change");
            aeinfoEl.setAttribute("access", "read");
            aeinfoEl.setAttribute("type", "string");
            QDomText aText = doc.createTextNode(QString::fromStdString(aiex.events.ch_event.abs_change));
            aeinfoEl.appendChild(aText);
            root.appendChild(aeinfoEl);
        }
        /* b2. struct PeriodicEventInfo */
        {
            QDomElement aeinfoEl = doc.createElement("periodicEvent_period");
            aeinfoEl.setAttribute("access", "read");
            aeinfoEl.setAttribute("type", "string");
            QDomText aText = doc.createTextNode(QString::fromStdString(aiex.events.per_event.period));
            aeinfoEl.appendChild(aText);
            root.appendChild(aeinfoEl);
        }
        /* b3. struct ArchiveEventInfo */
        {
            QDomElement aeinfoEl = doc.createElement("archive_rel_change");
            aeinfoEl.setAttribute("access", "read");
            aeinfoEl.setAttribute("type", "string");
            QDomText aText = doc.createTextNode(QString::fromStdString(aiex.events.arch_event.archive_rel_change));
            aeinfoEl.appendChild(aText);
            root.appendChild(aeinfoEl);
        }
        {
            QDomElement aeinfoEl = doc.createElement("archive_abs_change");
            aeinfoEl.setAttribute("access", "read");
            aeinfoEl.setAttribute("type", "string");
            QDomText aText = doc.createTextNode(QString::fromStdString(aiex.events.arch_event.archive_abs_change));
            aeinfoEl.appendChild(aText);
            root.appendChild(aeinfoEl);
        }
        {
            QDomElement aeinfoEl = doc.createElement("archive_period");
            aeinfoEl.setAttribute("access", "read");
            aeinfoEl.setAttribute("type", "string");
            QDomText aText = doc.createTextNode(QString::fromStdString(aiex.events.arch_event.archive_period));
            aeinfoEl.appendChild(aText);
            root.appendChild(aeinfoEl);
        }
}


    doc.appendChild(root);
    return doc.toString();
}

QString TActionDesc::toXml() const
{
    QDomDocument doc("TActionShortDescription");
    QDomElement root = doc.createElement("ta");
    QDomElement src = doc.createElement("pt");
    root.appendChild(src);
    QDomText srcText = doc.createTextNode(m_act->source());
    src.appendChild(srcText);

    QDomElement mode = doc.createElement("drm");
    QDomText modeTxt  = doc.createTextNode(QString::number(m_act->desiredRefreshMode()));
    root.appendChild(mode);
    mode.appendChild(modeTxt);

    QDomElement args = doc.createElement("args");
    QDomText argtxt = doc.createTextNode(m_act->args().join(","));
    args.appendChild(argtxt);
    root.appendChild(args);

    doc.appendChild(root);
    return doc.toString();
}


QString TActionDesc::m_boolToStr(bool f)const
{
    QString ret;
    f ? ret = "true" : ret = "false";
    return ret;
}

QString TActionDesc::m_quality(int q)const
{
    switch(q)
    {
    case ATTR_VALID:
        return "ATTR_VALID";
    case ATTR_INVALID:
        return "ATTR_INVALID";
    case ATTR_ALARM:
        return "ATTR_ALARM";
    case ATTR_CHANGING:
        return "ATTR_CHANGING";
    case ATTR_WARNING:
        return "ATTR_WARNING";
    default:
        return QString::number(q);
    }
}

QString TActionDesc::m_type(int typ) const
{
    switch(typ)
    {
    case COMMAND:
        return "command";
    default:
        return "attribute";
    }
}

QString TActionDesc::m_argToString(int type) const
{
    return QString(CmdArgTypeName[type]);
}

QString TActionDesc::m_attrWriteTypeToString(const AttrWriteType awt) const
{
    switch(awt)
    {
    case Tango::READ:
        return "READ";
    case Tango::READ_WITH_WRITE:
        return "READ_WITH_WRITE";
    case Tango::WRITE:
        return "WRITE";
    case Tango::READ_WRITE:
        return "READ_WRITE";
    default:
        return "INVALID ATTR WRITE TYPE";
    }
}

QString TActionDesc::m_attrDataFormatToString(const AttrDataFormat adf) const
{
    switch(adf)
    {
    case Tango::SCALAR:
        return "SCALAR";
    case Tango::SPECTRUM:
        return "SPECTRUM";
    case Tango::IMAGE:
        return "IMAGE";
    default:
        return "FMT_UNKNOWN";
    }
}

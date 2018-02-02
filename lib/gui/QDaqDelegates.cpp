#include "QDaqDelegates.h"
#include "core/QDaqChannel.h"

#include <QMetaProperty>
// display widgets
#include <QLabel>
#include <QLCDNumber>
//#include <qwt_thermo.h>
// input widgets
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QAbstractSlider>
#include <QAbstractButton>

WidgetVariant::WidgetVariant(QWidget* w) : widget_(w), type_(qNone)
{
	if (qobject_cast<QLabel*>(w)) type_ = qLabel;
	else if (qobject_cast<QLCDNumber*>(w)) type_ = qLCDNumber;
    //else if (qobject_cast<QwtThermo*>(w)) type_ = qwtThermo;
	else if (qobject_cast<QAbstractButton*>(w)) type_ =  qAbstractButton;
	else if (qobject_cast<QAbstractSlider*>(w)) type_ = qAbstractSlider;
	else if (qobject_cast<QSpinBox*>(w)) type_ = qSpinBox;
	else if (qobject_cast<QDoubleSpinBox*>(w)) type_ = qDoubleSpinBox;
	else if (qobject_cast<QLineEdit*>(w)) type_ = qLineEdit;
	else if (qobject_cast<QComboBox*>(w)) type_ = qComboBox;
	else if (w) type_ = qUnsupported;
}
bool WidgetVariant::canConvert(QVariant::Type t) const
{
	switch(t)
	{
	case QVariant::Bool:
		return (type_==qAbstractButton);
	case QVariant::Double:
		return (type_==qDoubleSpinBox) || 
			(type_==qLineEdit) || 
			(type_==qLabel) || 
            (type_==qLCDNumber); // ||
            //(type_==qwtThermo);
	case QVariant::Int:
	case QVariant::UInt:
		return (type_==qSpinBox) || 
			(type_==qAbstractSlider) || 
			(type_==qLCDNumber) ||
            //(type_==qwtThermo) ||
			(type_==qLabel) ||
			(type_==qLineEdit);
	case QVariant::String:
		return (type_==qLineEdit) || (type_==qLabel);
	default:
		return false;
	}
}
QVariant WidgetVariant::read()
{
	QWidget* w = widget_;
	if (!(w && isValid())) return QVariant();

	switch(type_)
	{
	case qAbstractButton:    
		return QVariant(
			((QAbstractButton*)(w))->isChecked()
			);
	case qAbstractSlider:
		return QVariant(
			((QAbstractSlider*)(w))->value()
			);
	case qSpinBox:
		return QVariant(
			((QSpinBox*)(w))->value()
			);
	case qDoubleSpinBox:
		return QVariant(
			((QDoubleSpinBox*)(w))->value()
			);
	case qLineEdit:
		return QVariant(
			((QLineEdit*)(w))->text()
			);
	case qComboBox:
		return QVariant(
			((QComboBox*)(w))->currentIndex()
			);
	case qLabel:
		return QVariant(
			((QLabel*)(w))->text()
			);
	case qLCDNumber:
		return QVariant(
			((QLCDNumber*)(w))->value()
			);
//	case qwtThermo:
//		return QVariant(
//			((QwtThermo*)(w))->value()
//			);
    default:
        break;
	}
	return QVariant();
}
void WidgetVariant::write(const QVariant& v)
{
	QWidget* w = widget_;
	if (!w) return;
	if (!v.isValid()) return;

	switch(type_)
	{
	case qAbstractButton:
		((QAbstractButton*)(w))->setChecked(v.toBool());
		break;
	case qAbstractSlider:
		((QAbstractSlider*)(w))->setValue(v.toInt());
		break;
	case qSpinBox:
		((QSpinBox*)(w))->setValue(v.toInt());
		break;
	case qDoubleSpinBox:
		((QDoubleSpinBox*)(w))->setValue(v.toDouble());
		break;
	case qLineEdit:
		((QLineEdit*)(w))->setText(v.toString());
		break;
	case qLabel:
		((QLabel*)(w))->setText(v.toString());
		break;
	case qLCDNumber:
		switch(v.type())
		{
		case QVariant::Double:
			((QLCDNumber*)(w))->display(v.toDouble());
			break;
		case QVariant::Int:
		case QVariant::UInt:
			((QLCDNumber*)(w))->display(v.toInt());
			break;
		default:
			((QLCDNumber*)(w))->display(v.toString());
		}
		break;
//	case qwtThermo:
//		((QwtThermo*)(w))->setValue(v.toDouble());
//		break;
    default:
        break;
	}
}
const char* WidgetVariant::widgetSignal() const
{
	const QWidget* w = widget_;
	if (!w) return 0;

	//const QMetaObject* metaObj = w->metaObject();

	switch(type_)
	{
	case qAbstractButton:
		return SIGNAL(toggled(bool));
	case qAbstractSlider:
		return SIGNAL(sliderReleased());
	case qSpinBox:
	case qDoubleSpinBox:
	case qLineEdit:
		return SIGNAL(editingFinished());
	default:
		break;
	}
	return 0;
}
/**************************************************************************************************/
DisplayDelegate::DisplayDelegate(QWidget* w, QDaqChannel* ch) :
    QObject(w),
    widget_(w),
    channel_(ch)
{
    connect(ch,SIGNAL(updateWidgets()),this,SLOT(updateDisplay()));
    connect(ch,SIGNAL(destroyed()),this,SLOT(deleteLater()));
}
DisplayDelegate::~DisplayDelegate()
{
}
void DisplayDelegate::updateDisplay()
{
//	if (widget_.isNull())
//	{
//		deleteLater();
//		return;
//	}

	if (widget_.isText()) widget_.write(channel_->formatedValue());
	else widget_.write(channel_->value());
}

//************************************************************************************//

PropertyDelegate::PropertyDelegate(QWidget* w, QDaqObject* obj, QMetaProperty p, bool readOnly) : QObject(w),
obj_(obj),property_(p),widget_(w), readOnly_(readOnly)
{
	// connect with object
	connect(obj,SIGNAL(propertiesChanged()),this,SLOT(objectPropertyChange()));
	// connect to widget
	connect(w,widget_.widgetSignal(),this,SLOT(widgetPropertyChange()));

    // if obj dies, I die too
	connect(obj_,SIGNAL(destroyed()),this,SLOT(deleteLater()));

	objectPropertyChange();
}
PropertyDelegate::~PropertyDelegate()
{
}
void PropertyDelegate::objectPropertyChange()
{
	widget_.write(getObjectValue());
}
void PropertyDelegate::widgetPropertyChange()
{
	if (!readOnly_) setObjectValue(widget_.read());
	widget_.write(getObjectValue());
}
QVariant PropertyDelegate::getObjectValue()
{
	return property_.read(obj_);
}
void PropertyDelegate::setObjectValue(const QVariant& v)
{
	if (property_.isWritable() && v.isValid()) property_.write(obj_,v);
}

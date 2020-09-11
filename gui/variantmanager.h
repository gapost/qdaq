#ifndef VARIANTMANAGER_H
#define VARIANTMANAGER_H

#include "qtvariantproperty.h"

class VariantManager : public QtVariantPropertyManager
{
    Q_OBJECT
public:
    VariantManager(QObject *parent = 0);
    ~VariantManager();

    virtual QVariant value(const QtProperty *property) const;
    virtual int valueType(int propertyType) const;
    virtual bool isPropertyTypeSupported(int propertyType) const;

    QString valueText(const QtProperty *property) const;

    public slots:
    virtual void setValue(QtProperty *property, const QVariant &val);
    protected:
    virtual void initializeProperty(QtProperty *property);
    virtual void uninitializeProperty(QtProperty *property);
};

#endif // VARIANTMANAGER_H

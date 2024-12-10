#pragma once

#include <QtCore/QObject>
#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>
#include <QtQml/QQmlParserStatus>
#include <QtQmlIntegration/QtQmlIntegration>

class Dimensions : public QObject, public QQmlParserStatus {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(qreal scale READ scale WRITE setScale NOTIFY scaleChanged)
    Q_PROPERTY(Type type READ type WRITE setType NOTIFY typeChanged)
    Q_INTERFACES(QQmlParserStatus)
public:
    enum class Type : uint8_t {
        Desktop,
        Phone
     };

    Q_ENUM(Type);

    explicit Dimensions(QObject *parent = nullptr) Q_DECL_NOTHROW;
    Q_INVOKABLE qint32 mm(qreal milimeters) const;

    qreal scale() const;
    void setScale(qreal scale);

    Type type() const;
    void setType(Type type);

    void toggleOrientation();

    // from QQmlParserStatus
    void classBegin() override;
    void componentComplete() override;

Q_SIGNALS:
    void scaleChanged();
    void typeChanged();
    void onPageUp();
    void onPageDown();
    void onHome();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    qint32 height() const;
    qint32 width() const;
    void recalulateDotsPerMilimeters();

    QSettings m_settings;
    qreal m_scale { 1.0 };
    Type m_type { Type::Desktop };
    qreal m_height_millimeters { 0.0 };
    qreal m_width_millimeters { 0.0 };
    // Actual millimeter value.
    qreal m_physical_dots_per_millimeter { 0.0 };
    // Actual millimeter value * device pixel ratio * m_scale.
    qreal m_device_dots_per_millimeter { 0.0 };
};

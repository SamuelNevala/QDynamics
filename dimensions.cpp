#include "dimensions.h"

#include <QtGui/QKeyEvent>
#include <QtGui/QWindow>

#include <QtMath>

static const qreal phone_height_milimeters = 150.5;
static const qreal phone_width_milimeters = 77.8;
static const qreal desktop_height_milimeters = 233;
static const qreal desktop_width_milimeters = 452;

Dimensions::Dimensions(QObject *parent) Q_DECL_NOTHROW
    : QObject(parent) {
#ifdef _DEBUG
    qInfo()<< "Simulation key override ON!";
    qInfo()<< "PageUp increments scale.";
    qInfo()<< "PageDown decrements scale.";
    qInfo()<< "Home toggles orientation.";
    qGuiApp->installEventFilter(this);
#endif
}

qint32 Dimensions::mm(qreal milimeters) const {
    return qCeil(milimeters * m_physical_dots_per_millimeter * qGuiApp->devicePixelRatio() * scale());
}

qreal Dimensions::scale() const {
    return m_scale;
}

void Dimensions::setScale(qreal scale) {
    if (qFuzzyCompare(scale, m_scale)) {
        return;
    }

    m_scale = scale;
    m_settings.setValue("scale", m_scale);
    // TODO figure out why?
    if (m_type != Type::Desktop) {
        auto window = qGuiApp->topLevelWindows().at(0);
        window->setHeight(height());
        window->setWidth(width());
    }

    Q_EMIT scaleChanged();
}

Dimensions::Type Dimensions::type() const {
    return m_type;
}

void Dimensions::setType(Dimensions::Type type) {
    if (type == m_type) {
        return;
    }

    m_type = type;
    switch (type) {
    case Type::Desktop:
        m_height_millimeters = desktop_height_milimeters;
        m_width_millimeters = desktop_width_milimeters;
        break;
    case Type::Phone:
        m_height_millimeters = phone_height_milimeters;
        m_width_millimeters = phone_width_milimeters;
        break;
    default:
        break;
    }

    auto window = qGuiApp->topLevelWindows().at(0);
    window->setHeight(height());
    window->setWidth(width());

    Q_EMIT typeChanged();
}

void Dimensions::toggleOrientation() {
    auto window = qGuiApp->topLevelWindows().at(0);
    auto content_orientation = window->contentOrientation();

    switch (type()) {
    case Type::Desktop:
        switch (content_orientation) {
        case Qt::ScreenOrientation::PrimaryOrientation:
        case Qt::ScreenOrientation::LandscapeOrientation:
            content_orientation = Qt::ScreenOrientation::PortraitOrientation;
            break;
        case Qt::ScreenOrientation::PortraitOrientation:
            content_orientation = Qt::ScreenOrientation::PrimaryOrientation;
            break;
        default:
            break;
        }
        break;
    case Type::Phone:
        switch (content_orientation) {
        case Qt::ScreenOrientation::PrimaryOrientation:
        case Qt::ScreenOrientation::PortraitOrientation:
            content_orientation = Qt::ScreenOrientation::LandscapeOrientation;
            break;
        case Qt::ScreenOrientation::LandscapeOrientation:
            content_orientation = Qt::ScreenOrientation::PrimaryOrientation;
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }

    if (content_orientation == window->contentOrientation()) {
        return;
    }

    window->reportContentOrientationChange(content_orientation);
    window->setHeight(height());
    window->setWidth(width());
}

void Dimensions::classBegin() {
}

void Dimensions::componentComplete() {
    connect(this, &Dimensions::onPageDown, [this] { setScale(m_scale - 0.01); });
    connect(this, &Dimensions::onPageUp, [this] { setScale(m_scale + 0.01); });
    connect(this, &Dimensions::onHome, [this] { toggleOrientation(); });

    auto window = qGuiApp->topLevelWindows().at(0);
    connect(window, &QWindow::xChanged, this, [this] { m_settings.setValue("x", qGuiApp->topLevelWindows().at(0)->x()); });
    connect(window, &QWindow::yChanged, this, [this] { m_settings.setValue("y", qGuiApp->topLevelWindows().at(0)->y()); });
    connect(window, &QWindow::heightChanged, this, [this] { m_settings.setValue("height", qGuiApp->topLevelWindows().at(0)->height()); });
    connect(window, &QWindow::widthChanged, this, [this] { m_settings.setValue("width", qGuiApp->topLevelWindows().at(0)->width()); });

    // //m_orientation = m_settings.value("orientation", Qt::ScreenOrientation::PrimaryOrientation).value<Qt::ScreenOrientation>();
    m_scale = m_settings.value("scale", 1.0f).toReal();
    recalulateDotsPerMilimeters();
    const auto screen = qGuiApp->primaryScreen();
    connect(screen, &QScreen::physicalDotsPerInchChanged, this, [this] { recalulateDotsPerMilimeters(); });
    connect(this, &Dimensions::scaleChanged, [this] { recalulateDotsPerMilimeters(); });

    window->setX(m_settings.value("x", 0).toInt());
    window->setY(m_settings.value("y", 0).toInt());
    window->setHeight(m_settings.value("height", height()).toInt());
    window->setWidth(m_settings.value("width", width()).toInt());
}

bool Dimensions::eventFilter(QObject *obj, QEvent *event) {
    Q_UNUSED(obj)
    if (!event || event->type() != QEvent::KeyRelease) {
        return QObject::eventFilter(obj, event);
    }
    auto keyEvent = static_cast<QKeyEvent *>(event);
    if (keyEvent->key() == Qt::Key_PageUp) {
        Q_EMIT onPageUp();
        return true;
    } else if (keyEvent->key() == Qt::Key_PageDown) {
        Q_EMIT onPageDown();
        return true;
    } else if (keyEvent->key() == Qt::Key_Home) {
        Q_EMIT onHome();
        return true;
    }
    return QObject::eventFilter(obj, event);
}

qint32 Dimensions::height() const {
    //const auto milimeters = m_orientation == Qt::ScreenOrientation::LandscapeOrientation ? m_width_millimeters : m_height_milimeters;
    return mm(m_height_millimeters);
}

qint32 Dimensions::width() const {
    //const auto milimeters = m_orientation == Qt::ScreenOrientation::LandscapeOrientation ? m_height_milimeters : m_width_millimeters;
    return mm(m_width_millimeters);
}

void Dimensions::recalulateDotsPerMilimeters() {
    const auto screen = qGuiApp->primaryScreen();
    m_physical_dots_per_millimeter = screen->physicalDotsPerInch() * 0.03937007874016;
    m_device_dots_per_millimeter = m_physical_dots_per_millimeter * screen->devicePixelRatio() * m_scale;
    qInfo()<< "Device pixel ratio:" << qGuiApp->devicePixelRatio();
    qInfo()<< "Scale:" << m_scale;
    qInfo()<< "Physical dots per inch:" << qGuiApp->primaryScreen()->physicalDotsPerInch();
    qInfo()<< "Physical dots per millimeter:" << m_physical_dots_per_millimeter;
    qInfo()<< "Device dots per millimeter:" << m_device_dots_per_millimeter;
}

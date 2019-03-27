/****************************************************************************
** Meta object code from reading C++ file 'tinstallerinfo.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../tinstallerinfo.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'tinstallerinfo.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_TComponent_t {
    QByteArrayData data[6];
    char stringdata0[52];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TComponent_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TComponent_t qt_meta_stringdata_TComponent = {
    {
QT_MOC_LITERAL(0, 0, 10), // "TComponent"
QT_MOC_LITERAL(1, 11, 11), // "nameChanged"
QT_MOC_LITERAL(2, 23, 0), // ""
QT_MOC_LITERAL(3, 24, 14), // "checkedChanged"
QT_MOC_LITERAL(4, 39, 4), // "name"
QT_MOC_LITERAL(5, 44, 7) // "checked"

    },
    "TComponent\0nameChanged\0\0checkedChanged\0"
    "name\0checked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TComponent[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       2,   26, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   24,    2, 0x06 /* Public */,
       3,    0,   25,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,

 // properties: name, type, flags
       4, QMetaType::QString, 0x00495003,
       5, QMetaType::Bool, 0x00495103,

 // properties: notify_signal_id
       0,
       1,

       0        // eod
};

void TComponent::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<TComponent *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->nameChanged(); break;
        case 1: _t->checkedChanged(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (TComponent::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TComponent::nameChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (TComponent::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TComponent::checkedChanged)) {
                *result = 1;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<TComponent *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = _t->name; break;
        case 1: *reinterpret_cast< bool*>(_v) = _t->checked; break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<TComponent *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0:
            if (_t->name != *reinterpret_cast< QString*>(_v)) {
                _t->name = *reinterpret_cast< QString*>(_v);
                Q_EMIT _t->nameChanged();
            }
            break;
        case 1: _t->setChecked(*reinterpret_cast< bool*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject TComponent::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_TComponent.data,
    qt_meta_data_TComponent,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *TComponent::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TComponent::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_TComponent.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int TComponent::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
#ifndef QT_NO_PROPERTIES
   else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 2;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void TComponent::nameChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void TComponent::checkedChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
struct qt_meta_stringdata_TInstallerInfo_t {
    QByteArrayData data[50];
    char stringdata0[715];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TInstallerInfo_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TInstallerInfo_t qt_meta_stringdata_TInstallerInfo = {
    {
QT_MOC_LITERAL(0, 0, 14), // "TInstallerInfo"
QT_MOC_LITERAL(1, 15, 22), // "applicationNameChanged"
QT_MOC_LITERAL(2, 38, 0), // ""
QT_MOC_LITERAL(3, 39, 14), // "appDescChanged"
QT_MOC_LITERAL(4, 54, 18), // "releaseSizeChanged"
QT_MOC_LITERAL(5, 73, 17), // "themeColorChanged"
QT_MOC_LITERAL(6, 91, 16), // "sizeStatsChanged"
QT_MOC_LITERAL(7, 108, 24), // "destinationFolderChanged"
QT_MOC_LITERAL(8, 133, 15), // "progressChanged"
QT_MOC_LITERAL(9, 149, 18), // "installationFailed"
QT_MOC_LITERAL(10, 168, 3), // "msg"
QT_MOC_LITERAL(11, 172, 21), // "installationCompleted"
QT_MOC_LITERAL(12, 194, 20), // "statusMessageChanged"
QT_MOC_LITERAL(13, 215, 4), // "info"
QT_MOC_LITERAL(14, 220, 11), // "requirement"
QT_MOC_LITERAL(15, 232, 6), // "osText"
QT_MOC_LITERAL(16, 239, 15), // "buildNumberText"
QT_MOC_LITERAL(17, 255, 7), // "cpuText"
QT_MOC_LITERAL(18, 263, 16), // "cpuFrequencyText"
QT_MOC_LITERAL(19, 280, 7), // "ramSize"
QT_MOC_LITERAL(20, 288, 8), // "ramUsage"
QT_MOC_LITERAL(21, 297, 7), // "gpuText"
QT_MOC_LITERAL(22, 305, 8), // "cpuUsage"
QT_MOC_LITERAL(23, 314, 14), // "expandConstant"
QT_MOC_LITERAL(24, 329, 8), // "aboutTxt"
QT_MOC_LITERAL(25, 338, 13), // "remainingTime"
QT_MOC_LITERAL(26, 352, 9), // "totalTime"
QT_MOC_LITERAL(27, 362, 17), // "startInstallation"
QT_MOC_LITERAL(28, 380, 9), // "threadUrl"
QT_MOC_LITERAL(29, 390, 11), // "facebookUrl"
QT_MOC_LITERAL(30, 402, 10), // "websiteUrl"
QT_MOC_LITERAL(31, 413, 9), // "timeToStr"
QT_MOC_LITERAL(32, 423, 1), // "s"
QT_MOC_LITERAL(33, 425, 17), // "pauseInstallation"
QT_MOC_LITERAL(34, 443, 18), // "resumeInstallation"
QT_MOC_LITERAL(35, 462, 21), // "terminateInstallation"
QT_MOC_LITERAL(36, 484, 23), // "isTerminateInstallation"
QT_MOC_LITERAL(37, 508, 11), // "bytesPerSec"
QT_MOC_LITERAL(38, 520, 11), // "getProgress"
QT_MOC_LITERAL(39, 532, 20), // "isInstallationFailed"
QT_MOC_LITERAL(40, 553, 15), // "applicationName"
QT_MOC_LITERAL(41, 569, 22), // "applicationDescription"
QT_MOC_LITERAL(42, 592, 17), // "destinationFolder"
QT_MOC_LITERAL(43, 610, 10), // "themeColor"
QT_MOC_LITERAL(44, 621, 12), // "requiredSize"
QT_MOC_LITERAL(45, 634, 14), // "diskTotalSpace"
QT_MOC_LITERAL(46, 649, 13), // "diskFreeSpace"
QT_MOC_LITERAL(47, 663, 28), // "hibernatePCAfterInstallation"
QT_MOC_LITERAL(48, 692, 8), // "progress"
QT_MOC_LITERAL(49, 701, 13) // "statusMessage"

    },
    "TInstallerInfo\0applicationNameChanged\0"
    "\0appDescChanged\0releaseSizeChanged\0"
    "themeColorChanged\0sizeStatsChanged\0"
    "destinationFolderChanged\0progressChanged\0"
    "installationFailed\0msg\0installationCompleted\0"
    "statusMessageChanged\0info\0requirement\0"
    "osText\0buildNumberText\0cpuText\0"
    "cpuFrequencyText\0ramSize\0ramUsage\0"
    "gpuText\0cpuUsage\0expandConstant\0"
    "aboutTxt\0remainingTime\0totalTime\0"
    "startInstallation\0threadUrl\0facebookUrl\0"
    "websiteUrl\0timeToStr\0s\0pauseInstallation\0"
    "resumeInstallation\0terminateInstallation\0"
    "isTerminateInstallation\0bytesPerSec\0"
    "getProgress\0isInstallationFailed\0"
    "applicationName\0applicationDescription\0"
    "destinationFolder\0themeColor\0requiredSize\0"
    "diskTotalSpace\0diskFreeSpace\0"
    "hibernatePCAfterInstallation\0progress\0"
    "statusMessage"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TInstallerInfo[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      36,   14, // methods
      10,  242, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      10,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,  194,    2, 0x06 /* Public */,
       3,    0,  195,    2, 0x06 /* Public */,
       4,    0,  196,    2, 0x06 /* Public */,
       5,    0,  197,    2, 0x06 /* Public */,
       6,    0,  198,    2, 0x06 /* Public */,
       7,    0,  199,    2, 0x06 /* Public */,
       8,    0,  200,    2, 0x06 /* Public */,
       9,    1,  201,    2, 0x06 /* Public */,
      11,    1,  204,    2, 0x06 /* Public */,
      12,    0,  207,    2, 0x06 /* Public */,

 // methods: name, argc, parameters, tag, flags
      13,    1,  208,    2, 0x02 /* Public */,
      14,    1,  211,    2, 0x02 /* Public */,
      15,    0,  214,    2, 0x02 /* Public */,
      16,    0,  215,    2, 0x02 /* Public */,
      17,    0,  216,    2, 0x02 /* Public */,
      18,    0,  217,    2, 0x02 /* Public */,
      19,    0,  218,    2, 0x02 /* Public */,
      20,    0,  219,    2, 0x02 /* Public */,
      21,    0,  220,    2, 0x02 /* Public */,
      22,    0,  221,    2, 0x02 /* Public */,
      23,    1,  222,    2, 0x02 /* Public */,
      24,    0,  225,    2, 0x02 /* Public */,
      25,    0,  226,    2, 0x02 /* Public */,
      26,    0,  227,    2, 0x02 /* Public */,
      27,    0,  228,    2, 0x02 /* Public */,
      28,    0,  229,    2, 0x02 /* Public */,
      29,    0,  230,    2, 0x02 /* Public */,
      30,    0,  231,    2, 0x02 /* Public */,
      31,    1,  232,    2, 0x02 /* Public */,
      33,    0,  235,    2, 0x02 /* Public */,
      34,    0,  236,    2, 0x02 /* Public */,
      35,    0,  237,    2, 0x02 /* Public */,
      36,    0,  238,    2, 0x02 /* Public */,
      37,    0,  239,    2, 0x02 /* Public */,
      38,    0,  240,    2, 0x02 /* Public */,
      39,    0,  241,    2, 0x02 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   10,
    QMetaType::Void, QMetaType::QString,   10,
    QMetaType::Void,

 // methods: parameters
    QMetaType::QString, QMetaType::QString,    2,
    QMetaType::QString, QMetaType::QString,    2,
    QMetaType::QString,
    QMetaType::QString,
    QMetaType::QString,
    QMetaType::QString,
    QMetaType::Int,
    QMetaType::Int,
    QMetaType::QString,
    QMetaType::Int,
    QMetaType::QString, QMetaType::QString,    2,
    QMetaType::QString,
    QMetaType::Int,
    QMetaType::Int,
    QMetaType::Void,
    QMetaType::QString,
    QMetaType::QString,
    QMetaType::QString,
    QMetaType::QString, QMetaType::LongLong,   32,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Bool,
    QMetaType::Int,
    QMetaType::Double,
    QMetaType::Bool,

 // properties: name, type, flags
      40, QMetaType::QString, 0x00495001,
      41, QMetaType::QString, 0x00495001,
      42, QMetaType::QString, 0x00495103,
      43, QMetaType::QColor, 0x00495001,
      44, QMetaType::Int, 0x00495001,
      45, QMetaType::Int, 0x00495001,
      46, QMetaType::Int, 0x00495001,
      47, QMetaType::Bool, 0x00095003,
      48, QMetaType::Double, 0x00495001,
      49, QMetaType::QString, 0x00495003,

 // properties: notify_signal_id
       0,
       1,
       5,
       3,
       4,
       4,
       4,
       0,
       6,
       9,

       0        // eod
};

void TInstallerInfo::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<TInstallerInfo *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->applicationNameChanged(); break;
        case 1: _t->appDescChanged(); break;
        case 2: _t->releaseSizeChanged(); break;
        case 3: _t->themeColorChanged(); break;
        case 4: _t->sizeStatsChanged(); break;
        case 5: _t->destinationFolderChanged(); break;
        case 6: _t->progressChanged(); break;
        case 7: _t->installationFailed((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 8: _t->installationCompleted((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 9: _t->statusMessageChanged(); break;
        case 10: { QString _r = _t->info((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 11: { QString _r = _t->requirement((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 12: { QString _r = _t->osText();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 13: { QString _r = _t->buildNumberText();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 14: { QString _r = _t->cpuText();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 15: { QString _r = _t->cpuFrequencyText();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 16: { int _r = _t->ramSize();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 17: { int _r = _t->ramUsage();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 18: { QString _r = _t->gpuText();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 19: { int _r = _t->cpuUsage();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 20: { QString _r = _t->expandConstant((*reinterpret_cast< QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 21: { QString _r = _t->aboutTxt();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 22: { int _r = _t->remainingTime();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 23: { int _r = _t->totalTime();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 24: _t->startInstallation(); break;
        case 25: { QString _r = _t->threadUrl();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 26: { QString _r = _t->facebookUrl();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 27: { QString _r = _t->websiteUrl();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 28: { QString _r = _t->timeToStr((*reinterpret_cast< long long(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 29: _t->pauseInstallation(); break;
        case 30: _t->resumeInstallation(); break;
        case 31: _t->terminateInstallation(); break;
        case 32: { bool _r = _t->isTerminateInstallation();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 33: { int _r = _t->bytesPerSec();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 34: { double _r = _t->getProgress();
            if (_a[0]) *reinterpret_cast< double*>(_a[0]) = std::move(_r); }  break;
        case 35: { bool _r = _t->isInstallationFailed();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (TInstallerInfo::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TInstallerInfo::applicationNameChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (TInstallerInfo::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TInstallerInfo::appDescChanged)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (TInstallerInfo::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TInstallerInfo::releaseSizeChanged)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (TInstallerInfo::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TInstallerInfo::themeColorChanged)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (TInstallerInfo::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TInstallerInfo::sizeStatsChanged)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (TInstallerInfo::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TInstallerInfo::destinationFolderChanged)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (TInstallerInfo::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TInstallerInfo::progressChanged)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (TInstallerInfo::*)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TInstallerInfo::installationFailed)) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (TInstallerInfo::*)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TInstallerInfo::installationCompleted)) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (TInstallerInfo::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TInstallerInfo::statusMessageChanged)) {
                *result = 9;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<TInstallerInfo *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = _t->applicationName(); break;
        case 1: *reinterpret_cast< QString*>(_v) = _t->applicationDescription(); break;
        case 2: *reinterpret_cast< QString*>(_v) = _t->destinationFolder(); break;
        case 3: *reinterpret_cast< QColor*>(_v) = _t->themeColor(); break;
        case 4: *reinterpret_cast< int*>(_v) = _t->requiredSize(); break;
        case 5: *reinterpret_cast< int*>(_v) = _t->diskTotalSpace(); break;
        case 6: *reinterpret_cast< int*>(_v) = _t->diskFreeSpace(); break;
        case 7: *reinterpret_cast< bool*>(_v) = _t->hibernatePCAfterInstallation_; break;
        case 8: *reinterpret_cast< double*>(_v) = _t->progress(); break;
        case 9: *reinterpret_cast< QString*>(_v) = _t->StatusMessage_; break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<TInstallerInfo *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 2: _t->setDestinationFolder(*reinterpret_cast< QString*>(_v)); break;
        case 7:
            if (_t->hibernatePCAfterInstallation_ != *reinterpret_cast< bool*>(_v)) {
                _t->hibernatePCAfterInstallation_ = *reinterpret_cast< bool*>(_v);
            }
            break;
        case 9:
            if (_t->StatusMessage_ != *reinterpret_cast< QString*>(_v)) {
                _t->StatusMessage_ = *reinterpret_cast< QString*>(_v);
                Q_EMIT _t->statusMessageChanged();
            }
            break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

QT_INIT_METAOBJECT const QMetaObject TInstallerInfo::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_TInstallerInfo.data,
    qt_meta_data_TInstallerInfo,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *TInstallerInfo::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TInstallerInfo::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_TInstallerInfo.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int TInstallerInfo::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 36)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 36;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 36)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 36;
    }
#ifndef QT_NO_PROPERTIES
   else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 10;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void TInstallerInfo::applicationNameChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void TInstallerInfo::appDescChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void TInstallerInfo::releaseSizeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void TInstallerInfo::themeColorChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void TInstallerInfo::sizeStatsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void TInstallerInfo::destinationFolderChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void TInstallerInfo::progressChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void TInstallerInfo::installationFailed(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void TInstallerInfo::installationCompleted(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void TInstallerInfo::statusMessageChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 9, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE

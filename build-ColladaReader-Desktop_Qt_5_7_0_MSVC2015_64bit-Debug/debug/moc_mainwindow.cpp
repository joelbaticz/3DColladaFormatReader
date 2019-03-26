/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../ColladaReader/mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[19];
    char stringdata0[366];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 23), // "on_actionOpen_triggered"
QT_MOC_LITERAL(2, 35, 0), // ""
QT_MOC_LITERAL(3, 36, 6), // "update"
QT_MOC_LITERAL(4, 43, 23), // "on_actionExit_triggered"
QT_MOC_LITERAL(5, 67, 23), // "on_actionQuit_triggered"
QT_MOC_LITERAL(6, 91, 24), // "on_actionClose_triggered"
QT_MOC_LITERAL(7, 116, 19), // "on_checkBox_toggled"
QT_MOC_LITERAL(8, 136, 7), // "checked"
QT_MOC_LITERAL(9, 144, 23), // "on_MainWindow_destroyed"
QT_MOC_LITERAL(10, 168, 21), // "on_pushButton_clicked"
QT_MOC_LITERAL(11, 190, 19), // "on_checkBox_clicked"
QT_MOC_LITERAL(12, 210, 21), // "on_checkBox_3_clicked"
QT_MOC_LITERAL(13, 232, 21), // "on_checkBox_4_clicked"
QT_MOC_LITERAL(14, 254, 21), // "on_checkBox_5_clicked"
QT_MOC_LITERAL(15, 276, 21), // "on_checkBox_6_clicked"
QT_MOC_LITERAL(16, 298, 21), // "on_checkBox_7_clicked"
QT_MOC_LITERAL(17, 320, 21), // "on_checkBox_2_clicked"
QT_MOC_LITERAL(18, 342, 23) // "on_pushButton_2_clicked"

    },
    "MainWindow\0on_actionOpen_triggered\0\0"
    "update\0on_actionExit_triggered\0"
    "on_actionQuit_triggered\0"
    "on_actionClose_triggered\0on_checkBox_toggled\0"
    "checked\0on_MainWindow_destroyed\0"
    "on_pushButton_clicked\0on_checkBox_clicked\0"
    "on_checkBox_3_clicked\0on_checkBox_4_clicked\0"
    "on_checkBox_5_clicked\0on_checkBox_6_clicked\0"
    "on_checkBox_7_clicked\0on_checkBox_2_clicked\0"
    "on_pushButton_2_clicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   94,    2, 0x0a /* Public */,
       3,    0,   95,    2, 0x0a /* Public */,
       4,    0,   96,    2, 0x0a /* Public */,
       5,    0,   97,    2, 0x0a /* Public */,
       6,    0,   98,    2, 0x0a /* Public */,
       7,    1,   99,    2, 0x0a /* Public */,
       9,    0,  102,    2, 0x0a /* Public */,
      10,    0,  103,    2, 0x0a /* Public */,
      11,    0,  104,    2, 0x08 /* Private */,
      12,    0,  105,    2, 0x08 /* Private */,
      13,    0,  106,    2, 0x08 /* Private */,
      14,    0,  107,    2, 0x08 /* Private */,
      15,    0,  108,    2, 0x08 /* Private */,
      16,    0,  109,    2, 0x08 /* Private */,
      17,    0,  110,    2, 0x08 /* Private */,
      18,    0,  111,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    8,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MainWindow *_t = static_cast<MainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_actionOpen_triggered(); break;
        case 1: _t->update(); break;
        case 2: _t->on_actionExit_triggered(); break;
        case 3: _t->on_actionQuit_triggered(); break;
        case 4: _t->on_actionClose_triggered(); break;
        case 5: _t->on_checkBox_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: _t->on_MainWindow_destroyed(); break;
        case 7: _t->on_pushButton_clicked(); break;
        case 8: _t->on_checkBox_clicked(); break;
        case 9: _t->on_checkBox_3_clicked(); break;
        case 10: _t->on_checkBox_4_clicked(); break;
        case 11: _t->on_checkBox_5_clicked(); break;
        case 12: _t->on_checkBox_6_clicked(); break;
        case 13: _t->on_checkBox_7_clicked(); break;
        case 14: _t->on_checkBox_2_clicked(); break;
        case 15: _t->on_pushButton_2_clicked(); break;
        default: ;
        }
    }
}

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow.data,
      qt_meta_data_MainWindow,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 16)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 16;
    }
    return _id;
}
QT_END_MOC_NAMESPACE

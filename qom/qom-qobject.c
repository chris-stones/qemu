/*
 * QEMU Object Model - QObject wrappers
 *
 * Copyright (C) 2012 Red Hat, Inc.
 *
 * Author: Paolo Bonzini <pbonzini@redhat.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or later.
 * See the COPYING file in the top-level directory.
 */

#include "qemu/osdep.h"
#include "qapi/error.h"
#include "qemu-common.h"
#include "qom/object.h"
#include "qom/qom-qobject.h"
#include "qapi/visitor.h"
#include "qapi/qmp-input-visitor.h"
#include "qapi/qmp-output-visitor.h"

#if defined(CONFIG_GNU_ARM_ECLIPSE)
#include "qapi/qmp/types.h"
#include "qemu/log.h"
#endif /* defined(CONFIG_GNU_ARM_ECLIPSE) */

#if defined(CONFIG_GNU_ARM_ECLIPSE__)
static char *dump_value(QObject *value, char *buf, size_t siz)
{
    if (value->type->code == QTYPE_QINT) {
        QInt *p = (QInt *) value;
        snprintf(buf, siz, "%" PRId64, p->value);
        return buf;
    } else if (value->type->code == QTYPE_QSTRING) {
        QString *p = (QString *) value;
        return p->string;
    } else if (value->type->code == QTYPE_QBOOL) {
        QBool *p = (QBool *) value;
        return (char*) (p->value ? "true" : "false");
    }
    return (char*) "?";
}
#endif

void object_property_set_qobject(Object *obj, QObject *value,
                                 const char *name, Error **errp)
{
    Visitor *v;
    /* TODO: Should we reject, rather than ignore, excess input? */
    v = qmp_input_visitor_new(value, false);
    object_property_set(obj, v, name, errp);
    visit_free(v);
}

QObject *object_property_get_qobject(Object *obj, const char *name,
                                     Error **errp)
{
    QObject *ret = NULL;
    Error *local_err = NULL;
    Visitor *v;

    v = qmp_output_visitor_new(&ret);
    object_property_get(obj, v, name, &local_err);
    if (!local_err) {
        visit_complete(v, &ret);
    }
    error_propagate(errp, local_err);
    visit_free(v);
    return ret;
}

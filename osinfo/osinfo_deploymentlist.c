/*
 * libosinfo:
 *
 * Copyright (C) 2009-2012 Red Hat, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Authors:
 *   Arjun Roy <arroy@redhat.com>
 *   Daniel P. Berrange <berrange@redhat.com>
 */

#include <osinfo/osinfo.h>

G_DEFINE_TYPE (OsinfoDeploymentList, osinfo_deploymentlist, OSINFO_TYPE_LIST);

#define OSINFO_DEPLOYMENTLIST_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), OSINFO_TYPE_DEPLOYMENTLIST, OsinfoDeploymentListPrivate))

/**
 * SECTION:osinfo_deploymentlist
 * @short_description: A list of hardware deployment
 * @see_also: #OsinfoList, #OsinfoDeployment
 *
 * #OsinfoDeploymentList is a list specialization that stores
 * only #OsinfoDeployment objects.
 */

struct _OsinfoDeploymentListPrivate
{
    gboolean unused;
};

static void
osinfo_deploymentlist_finalize (GObject *object)
{
    /* Chain up to the parent class */
    G_OBJECT_CLASS (osinfo_deploymentlist_parent_class)->finalize (object);
}

/* Init functions */
static void
osinfo_deploymentlist_class_init (OsinfoDeploymentListClass *klass)
{
    GObjectClass *g_klass = G_OBJECT_CLASS (klass);

    g_klass->finalize = osinfo_deploymentlist_finalize;
    g_type_class_add_private (klass, sizeof (OsinfoDeploymentListPrivate));
}

static void
osinfo_deploymentlist_init (OsinfoDeploymentList *list)
{
    OsinfoDeploymentListPrivate *priv;
    list->priv = priv = OSINFO_DEPLOYMENTLIST_GET_PRIVATE(list);

}


/**
 * osinfo_deploymentlist_new:
 *
 * Construct a new deployment list that is initially empty.
 *
 * Returns: (transfer full): an empty deployment list
 */
OsinfoDeploymentList *osinfo_deploymentlist_new(void)
{
    return g_object_new(OSINFO_TYPE_DEPLOYMENTLIST,
                        "element-type", OSINFO_TYPE_DEPLOYMENT,
                        NULL);
}


/**
 * osinfo_deploymentlist_new_copy:
 * @source: the deployment list to copy
 *
 * Construct a new deployment list that is filled with deployments
 * from @source
 *
 * Returns: (transfer full): a copy of the deployment list
 */
OsinfoDeploymentList *osinfo_deploymentlist_new_copy(OsinfoDeploymentList *source)
{
    OsinfoDeploymentList *newList = osinfo_deploymentlist_new();
    osinfo_list_add_all(OSINFO_LIST(newList),
                        OSINFO_LIST(source));
    return newList;
}


/**
 * osinfo_deploymentlist_new_filtered:
 * @source: the deployment list to copy
 * @filter: the filter to apply
 *
 * Construct a new deployment list that is filled with deployments
 * from @source that match @filter
 *
 * Returns: (transfer full): a filtered copy of the deployment list
 */
OsinfoDeploymentList *osinfo_deploymentlist_new_filtered(OsinfoDeploymentList *source, OsinfoFilter *filter)
{
    OsinfoDeploymentList *newList = osinfo_deploymentlist_new();
    osinfo_list_add_filtered(OSINFO_LIST(newList),
                             OSINFO_LIST(source),
                             filter);
    return newList;
}

/**
 * osinfo_deploymentlist_new_intersection:
 * @sourceOne: the first deployment list to copy
 * @sourceTwo: the second deployment list to copy
 *
 * Construct a new deployment list that is filled with only the
 * deployments that are present in both @sourceOne and @sourceTwo.
 *
 * Returns: (transfer full): an intersection of the two deployment lists
 */
OsinfoDeploymentList *osinfo_deploymentlist_new_intersection(OsinfoDeploymentList *sourceOne, OsinfoDeploymentList *sourceTwo)
{
    OsinfoDeploymentList *newList = osinfo_deploymentlist_new();
    osinfo_list_add_intersection(OSINFO_LIST(newList),
                                 OSINFO_LIST(sourceOne),
                                 OSINFO_LIST(sourceTwo));
    return newList;
}

/**
 * osinfo_deploymentlist_new_union:
 * @sourceOne: the first deployment list to copy
 * @sourceTwo: the second deployment list to copy
 *
 * Construct a new deployment list that is filled with all the
 * deployments that are present in either @sourceOne and @sourceTwo.
 *
 * Returns: (transfer full): a union of the two deployment lists
 */
OsinfoDeploymentList *osinfo_deploymentlist_new_union(OsinfoDeploymentList *sourceOne, OsinfoDeploymentList *sourceTwo)
{
    OsinfoDeploymentList *newList = osinfo_deploymentlist_new();
    osinfo_list_add_union(OSINFO_LIST(newList),
                          OSINFO_LIST(sourceOne),
                          OSINFO_LIST(sourceTwo));
    return newList;
}

/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 */

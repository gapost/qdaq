#include "h5helper_v1_1.h"

Group h5helper_v1_1::createGroup(CommonFG* loc, const char* name)
{
    hid_t group_creation_plist;
    group_creation_plist = H5Pcreate(H5P_GROUP_CREATE);
    herr_t status = H5Pset_link_creation_order(group_creation_plist,
                                     H5P_CRT_ORDER_TRACKED | H5P_CRT_ORDER_INDEXED);

    if (status<0) throw PropListIException("H5Pset_link_creation_order");

    hid_t group_id;
    group_id = H5Gcreate(loc->getLocId(),
                         name,
                         H5P_DEFAULT,
                         group_creation_plist,
                         H5P_DEFAULT);

    Group h5g(group_id);

    H5Pclose(group_creation_plist);
   //5Gclose(group_id);

    return h5g;
}

QByteArrayList h5helper_v1_1::getGroupNames(CommonFG* h5g, bool isRoot)
{
    QByteArrayList names;

    int n = h5g->getNumObjs();

    if (isRoot) {
        for(int i=0; i<n; ++i) {
            H5G_obj_t typ = h5g->getObjTypeByIdx(i);
            if (typ==H5G_GROUP)
            {
                QByteArray groupName;
                groupName.fill('\0',256);
                h5g->getObjnameByIdx(i,groupName.data(),256);
                names.push_back(groupName);
            }
        }
    } else {
        for(int i=0; i<n; ++i) {
            QByteArray groupName;
            groupName.fill('\0',256);

            herr_t ret = H5Lget_name_by_idx(h5g->getLocId(), ".", H5_INDEX_CRT_ORDER, H5_ITER_INC,
                                            i, groupName.data(), 256, 0);
            if (ret<0) break;
            H5O_info_t object_info;
            ret = H5Oget_info_by_name(h5g->getLocId(), groupName, &object_info, 0);
            if (object_info.type == H5O_TYPE_GROUP) names.push_back(groupName);

        }
    }

    return names;
}


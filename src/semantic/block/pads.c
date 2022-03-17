#include "semantic.h"

/* pads block (save pad linking name in the future..) */

static void             semantic_block_pads_one(
                        list_t **linked_pads,
                        ast_node_t *node,
                        GstElement *element) {

    list_t              *deepblock = lstnew("to", sizeof(char) * 3);
    ast_node_t          *scalar_node = ast_iscalar_get_by_key(node, "from");
    ast_node_t          *scalar_name = NULL;
    ast_node_t          *scalar_pad = NULL;
    ast_node_t          *to_block = NULL;
    list_t              *linked_pad_lst = NULL;
    linked_pad_t        *linked_pad = (linked_pad_t *)malloc(sizeof(linked_pad_t));

    if (scalar_node == NULL)
        return ;
    to_block = ast_iblock_get(node, deepblock);
    scalar_name = ast_iscalar_get_by_key(to_block, "name");
    scalar_pad = ast_iscalar_get_by_key(to_block, "pad");

    linked_pad->src_element = element;
    linked_pad->src_pad_name = scalar_node->right->str;
    linked_pad->dst_element_name = scalar_name->right->str;
    linked_pad->dst_pad_name = scalar_pad->right->str;

    linked_pad_lst = lstnew(linked_pad, sizeof(linked_pad_t));
    lstaddlast(linked_pads, linked_pad_lst);
    ast_deepblock_free(deepblock);
}

void                    semantic_block_pads(
                        list_t **linked_pads,
                        ast_node_t *node,
                        GstElement *element) {

    node = node->left;
    while (node) {
        semantic_block_pads_one(linked_pads, node, element);
        node = node->right;
    }
}

static void             semantic_block_pad_prop_one(
                        ast_node_t *node,
                        GstElement *element) {

    ast_node_t          *pad = ast_iscalar_get_by_key(node, "pad");
    list_t              *prop_deepblock = lstnew("properties", sizeof(char) * 11);
    ast_node_t          *prop = ast_iblock_get(node, prop_deepblock);
    /* get multiple properties .. */
}

void                    semantic_block_pad_prop(
                        ast_node_t *node,
                        GstElement *element) {

    node = node->left;
    while (node) {
        semantic_block_pad_prop_one(node, element);
        node = node->right;
    }
}

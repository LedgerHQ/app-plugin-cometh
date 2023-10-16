#include <stdbool.h>
#include "cometh_plugin.h"

// Set UI for address screen.
static bool set_address_ui(char *title, ethQueryContractUI_t *msg, context_t *context) {
    strlcpy(msg->title, title, msg->titleLength);

    // Prefix the address with `0x`.
    msg->msg[0] = '0';
    msg->msg[1] = 'x';

    // We need a random chainID for legacy reasons with `getEthAddressStringFromBinary`.
    // Setting it to `0` will make it work with every chainID :)
    uint64_t chainid = 0;

    // Get the string representation of the address stored in `context->beneficiary`. Put it in
    // `msg->msg`.
    return getEthAddressStringFromBinary(
        context->address,
        msg->msg + 2,  // +2 here because we've already prefixed with '0x'.
        msg->pluginSharedRW->sha3,
        chainid);
}

// Set UI for Booster cards count
static bool set_craft_booster_ui(ethQueryContractUI_t *msg, const context_t *context) {
    strlcpy(msg->title, "Booster", msg->titleLength);
    if (context->booster_card_count > 1) {
        snprintf(msg->msg, msg->msgLength, "%d cards", context->booster_card_count);
    } else {
        snprintf(msg->msg, msg->msgLength, "%d card", context->booster_card_count);
    }
    return true;
}

// Set UI for Item ID
static bool set_item_id_ui(ethQueryContractUI_t *msg, const context_t *context) {
    strlcpy(msg->title, "Card ID", msg->titleLength);
    return amountToString(context->uint256_one, INT256_LENGTH, 0, "", msg->msg, msg->msgLength);
}

// Set UI for Game ID
static bool set_game_id_ui(ethQueryContractUI_t *msg, const context_t *context) {
    strlcpy(msg->title, "Game ID", msg->titleLength);
    strlcpy(msg->msg, context->uint256_one, msg->msgLength);
    return true;
}

// Set UI for Rental offer nonce
static bool set_rental_offer_bundle_size_ui(ethQueryContractUI_t *msg, const context_t *context) {
    // set title of the screen depending on public vs private bundle
    if (memcmp(context->address, NULL_ETH_ADDRESS, ADDRESS_LENGTH) == 0) {
        strlcpy(msg->title, "Public bundle size", msg->titleLength);
    } else {
        strlcpy(msg->title, "Private bundle size", msg->titleLength);
    }
    if (context->array_length > 1) {
        snprintf(msg->msg, msg->msgLength, "%d NFTs", context->array_length);
    } else {
        snprintf(msg->msg, msg->msgLength, "%d NFT", context->array_length);
    }
    return true;
}

// Set UI for Rental offer nonce
static bool set_rental_offer_fee_amount_ui(ethQueryContractUI_t *msg, const context_t *context) {
    strlcpy(msg->title, "Entry fee", msg->titleLength);

    uint8_t decimals = context->decimals;
    const char *ticker = context->ticker;

    if (context->token_found) {
        if (!amountToString(context->uint256_two,
                            sizeof(context->uint256_two),
                            decimals,
                            ticker,
                            msg->msg,
                            msg->msgLength)) {
            return false;
        }
    } else {
        strlcpy(msg->msg, "Unknown token", msg->msgLength);
    }
    return true;
}

// Set UI for Rental offer nonce
static bool set_rental_offer_nonce_ui(ethQueryContractUI_t *msg, const context_t *context) {
    strlcpy(msg->title, "Offer Nonce", msg->titleLength);
    return amountToString(context->uint256_one, INT256_LENGTH, 0, "", msg->msg, msg->msgLength);
}

// Set UI for Rental token ID
static bool set_rental_token_id_ui(ethQueryContractUI_t *msg, const context_t *context) {
    strlcpy(msg->title, "Ship ID", msg->titleLength);
    return amountToString(context->uint256_one, INT256_LENGTH, 0, "", msg->msg, msg->msgLength);
}

// Set UI for Rental basis points
static bool set_rental_basis_points_ui(ethQueryContractUI_t *msg, const context_t *context) {
    strlcpy(msg->title, "Percentage lender", msg->titleLength);
    return amountToString(context->uint256_two, INT256_LENGTH, 2, "", msg->msg, msg->msgLength);
}

void handle_query_contract_ui(ethQueryContractUI_t *msg) {
    context_t *context = (context_t *) msg->pluginContext;
    bool ret = false;

    // Clean the display fields.
    memset(msg->title, 0, msg->titleLength);
    memset(msg->msg, 0, msg->msgLength);

    PRINTF("Switching to selector: %d and screen: %d\n", context->selectorIndex, msg->screenIndex);

    switch (context->selectorIndex) {
        case CRAFT:
            switch (msg->screenIndex) {
                case 0:
                    ret = set_address_ui("Beneficiary", msg, context);
                    break;
                case 1:
                    ret = set_craft_booster_ui(msg, context);
                    break;
            }
            break;
        case REDEEM:
            switch (msg->screenIndex) {
                case 0:
                    ret = set_address_ui("Beneficiary", msg, context);
                    break;
            }
            break;
        case GRIND:
            switch (msg->screenIndex) {
                case 0:
                    ret = set_item_id_ui(msg, context);
                    break;
            }
            break;
        case GET_REWARD:
            switch (msg->screenIndex) {
                case 0:
                    ret = set_game_id_ui(msg, context);
                    break;
            }
            break;
        case RENTAL_CREATE_OFFER:
            switch (msg->screenIndex) {
                case 0:
                    ret = set_rental_offer_bundle_size_ui(msg, context);
                    break;
                case 1:
                    ret = set_rental_offer_fee_amount_ui(msg, context);
                    break;
                case 2:
                    ret = set_rental_offer_nonce_ui(msg, context);
                    break;
                case 3:
                    ret = set_address_ui("Beneficiary", msg, context);
                    break;
            }
            break;
        case RENTAL_CANCEL_OFFER:
            switch (msg->screenIndex) {
                case 0:
                    ret = set_rental_offer_nonce_ui(msg, context);
                    break;
            }
            break;
        case RENTAL_RENT:
            switch (msg->screenIndex) {
                case 0:
                    ret = set_rental_offer_bundle_size_ui(msg, context);
                    break;
                case 1:
                    ret = set_rental_offer_fee_amount_ui(msg, context);
                    break;
                case 2:
                    ret = set_address_ui("Offer From", msg, context);
                    break;
            }
            break;
        case RENTAL_SUBLET:
            switch (msg->screenIndex) {
                case 0:
                    ret = set_rental_token_id_ui(msg, context);
                    break;
                case 1:
                    ret = set_address_ui("Tenant", msg, context);
                    break;
                case 2:
                    ret = set_rental_basis_points_ui(msg, context);
                    break;
            }
            break;
        case RENTAL_END_RENTAL:
        case RENTAL_END_RENTAL_PREMATURELY:
        case RENTAL_END_SUBLET:
            switch (msg->screenIndex) {
                case 0:
                    ret = set_rental_token_id_ui(msg, context);
                    break;
            }
            break;
        default:
            PRINTF("Received an invalid selectorIndex + screenIndex tuple\n");
    }
    msg->result = ret ? ETH_PLUGIN_RESULT_OK : ETH_PLUGIN_RESULT_ERROR;
}

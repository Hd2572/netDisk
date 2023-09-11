
#include "protocol.h"

PDU* mkPDU(uint uiMsgLen)  //申请空间
{
    uint uiPDULen = sizeof(PDU) + uiMsgLen;  //前面大小不变
    PDU* pdu = (PDU*)malloc(uiPDULen);

    if (NULL == pdu)
    {
        exit(EXIT_FAILURE);
    }
    memset(pdu, 0, uiPDULen);  //置0

    pdu->uiPDULen = uiPDULen;
    pdu->uiMsgLen = uiMsgLen;
    return pdu;
}

//实现所有PSS的PacketParse解析
//简化接口，以函数的方式实现
//add by freeeyes

#include "define.h"
#include "IMessageBlockManager.h"

#include "ace/svc_export.h"

#ifdef WIN32
#ifdef PACKETPARSE_INTERFACE_EXPORTS
#define DECLDIR __declspec(dllexport)
#else
#define DECLDIR __declspec(dllimport)
#endif
#else
#define DECLDIR ACE_Svc_Export
#endif

using namespace PSS;

extern "C"
{
    DECLDIR bool Parse_Packet_Head_Info(uint32 u4ConnectID, ACE_Message_Block* pmbHead, IMessageBlockManager* pMessageBlockManager, _Head_Info* pHeadInfo);
    DECLDIR bool Parse_Packet_Body_Info(uint32 u4ConnectID, ACE_Message_Block* pmbbody, IMessageBlockManager* pMessageBlockManager, _Body_Info* pBodyInfo);
    DECLDIR uint8 Parse_Packet_Stream(uint32 u4ConnectID, ACE_Message_Block* pCurrMessage, IMessageBlockManager* pMessageBlockManager, _Packet_Info* pPacketInfo);
    DECLDIR bool Make_Send_Packet(uint32 u4ConnectID, const char* pData, uint32 u4Len, ACE_Message_Block* pMbData, uint16 u2CommandID);
    DECLDIR uint32 Make_Send_Packet_Length(uint32 u4ConnectID, uint32 u4DataLen, uint16 u2CommandID);
    DECLDIR bool Connect(uint32 u4ConnectID, _ClientIPInfo objClientIPInfo, _ClientIPInfo objLocalIPInfo);
    DECLDIR void DisConnect(uint32 u4ConnectID);

    //解析包头，需要填充pHeadInfo数据结构，完成后填充_Head_Info的数据结构
    bool Parse_Packet_Head_Info(uint32 u4ConnectID, ACE_Message_Block* pmbHead, IMessageBlockManager* pMessageBlockManager, _Head_Info* pHeadInfo)
    {
        if(NULL == pHeadInfo || NULL == pMessageBlockManager)
        {
            return false;
        }

        //这里可以添加你的代码
        char* pData  = (char* )pmbHead->rd_ptr();
        uint32 u4Len = (uint32)pmbHead->length();
        uint32 u4Pos = 0;

        uint32 u2Version     = 0;           //协议版本号
        uint32 u2CmdID       = 0;           //CommandID
        uint32 u4BodyLen     = 0;           //包体长度
        char   szSession[33] = {'\0'};      //Session字符串

        //解析包头
        memcpy_safe((char* )&pData[u4Pos], (uint32)sizeof(uint16), (char* )&u2Version, (uint32)sizeof(uint16));
        u4Pos += sizeof(uint16);
        memcpy_safe((char* )&pData[u4Pos], (uint32)sizeof(uint16), (char* )&u2CmdID, (uint32)sizeof(uint16));
        u4Pos += sizeof(uint16);
        memcpy_safe((char* )&pData[u4Pos], (uint32)sizeof(uint32), (char* )&u4BodyLen, (uint32)sizeof(uint32));
        u4Pos += sizeof(uint32);
        memcpy_safe((char* )&pData[u4Pos], (uint32)(sizeof(char)*32), (char* )&szSession, (uint32)(sizeof(char)*32));
        u4Pos += sizeof(char)*32;

        OUR_DEBUG((LM_INFO,"[CPacketParse::SetPacketHead]m_u2Version=%d,m_u2CmdID=%d,m_u4BodyLen=%d.\n",
                   u2Version,
                   u2CmdID,
                   u4BodyLen));

        //填充返回给框架的数据包头信息
        pHeadInfo->m_u4HeadSrcLen      = u4Len;
        pHeadInfo->m_u4HeadCurrLen     = u4Len;
        pHeadInfo->m_u2PacketCommandID = u2CmdID;
        pHeadInfo->m_pmbHead           = pmbHead;
        pHeadInfo->m_u4BodySrcLen      = u4BodyLen;

        return true;
    }

    //解析包体，需要填充pBodyInfo数据结构，完成后填充_Body_Info的数据结构
    bool Parse_Packet_Body_Info(uint32 u4ConnectID, ACE_Message_Block* pmbbody, IMessageBlockManager* pMessageBlockManager, _Body_Info* pBodyInfo)
    {
        if(NULL == pBodyInfo || NULL == pMessageBlockManager)
        {
            return false;
        }

        //填充返回给框架的包体信息
        pBodyInfo->m_u4BodySrcLen  = (uint32)pmbbody->length();
        pBodyInfo->m_u4BodyCurrLen = (uint32)pmbbody->length();
        pBodyInfo->m_pmbBody       = pmbbody;

        return true;
    }

    //流模式据解析，解析成功需要填充_Packet_Info结构
    uint8 Parse_Packet_Stream(uint32 u4ConnectID, ACE_Message_Block* pCurrMessage, IMessageBlockManager* pMessageBlockManager, _Packet_Info* pPacketInfo)
    {
        //这里可以添加你的代码

        return 0;
    }

    //拼接数据返回包，所有的返回数据包都会调用这个
    bool Make_Send_Packet(uint32 u4ConnectID, const char* pData, uint32 u4Len, ACE_Message_Block* pMbData, uint16 u2CommandID)
    {
        if(pMbData == NULL && u2CommandID == 0)
        {
            return false;
        }

        //拼装数据包
        memcpy_safe((char* )&u4Len, (uint32)sizeof(uint32), pMbData->wr_ptr(), (uint32)sizeof(uint32));
        pMbData->wr_ptr(sizeof(uint32));
        memcpy_safe((char* )pData, u4Len, pMbData->wr_ptr(), u4Len);
        pMbData->wr_ptr(u4Len);

        return true;
    }

    //得到返回数据包的长度
    uint32 Make_Send_Packet_Length(uint32 u4ConnectID, uint32 u4DataLen, uint16 u2CommandID)
    {
        return u4DataLen + sizeof(uint32);
    }

    //当连接第一次建立的时候，返回的接口用于你自己的处理。
    bool Connect(uint32 u4ConnectID, _ClientIPInfo objClientIPInfo, _ClientIPInfo objLocalIPInfo)
    {
        return true;
    }

    //当连接断开的时候，返回你自己的处理
    void DisConnect(uint32 u4ConnectID)
    {

    }
}

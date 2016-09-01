#include "hw/hw.h"
#include "net/net.h"
#include "sysemu/sysemu.h"
#include "hw/sysbus.h"
#define TYPE_DWGMAC "dwmac"
#define trastate(a)	 (a<<20)
#define recestate(a)	 (a<<17)
#define NIS_MA	 (1<<16)
#define AIS_MA	 (1<<15)
#define ERI_MA	 (1<<14)
#define ETI_MA	 (1<<10)
#define RPS_MA	 (1<<8)
#define RU_MA	 (1<<7)
#define RI_MA	 (1<<6)
#define UNF_MA	 (1<<5)
#define OVF_MA	 (1<<4)
#define TU_MA	 (1<<2)
#define TPS_MA	 (1<<1)
#define TI_MA	 (1<<0)
#define MAC_MAX_FRAME_SZ	(1600)
#define DWGMAC(obj) OBJECT_CHECK(DWGMACState, (obj), TYPE_DWGMAC)

typedef struct DWGMACState {
	SysBusDevice parent_obj;
	MemoryRegion iomem;

	/*DWMACR macr;*/
	uint32_t conf;		/* 0x00 */
	uint32_t framefilt;		/* 0x04 */
	uint32_t hashtablehigh;	/* 0x08 */
	uint32_t hashtablelow;	/* 0x0c */
	uint32_t miiaddr;		/* 0x10 */
	uint32_t miidata;		/* 0x14 */
	uint32_t flowcontrol;	/* 0x18 */
	uint32_t vlantag;		/* 0x1c */
	uint32_t version;		/* 0x20 */
	uint32_t intreg;		/* 0x38 */
	uint32_t intmask;		/* 0x3c */
	uint32_t macaddr0hi;		/* 0x40 */
	uint32_t macaddr0lo;		/* 0x44 */

	/*DWDMAR dmar;*/
	uint32_t busmode;		/* 0x00 */
	uint32_t txpolldemand;	/* 0x04 */
	uint32_t rxpolldemand;	/* 0x08 */
	uint32_t *rxdesclistaddr;	/* 0x0c */
	uint32_t *txdesclistaddr;	/* 0x10 */
	uint32_t status;		/* 0x14 */
	uint32_t opmode;		/* 0x18 */
	uint32_t intenable;		/* 0x1c */
	uint32_t axibus;		/* 0x28 */
	uint32_t *currhosttxdesc;	/* 0x48 */
	uint32_t *currhostrxdesc;	/* 0x4c */
	uint32_t *currhosttxbuffaddr;	/* 0x50 */
	uint32_t *currhostrxbuffaddr;	/* 0x54 */

	qemu_irq irq;
	NICState *nic;
	NICConf c;
	uint16_t rnum;
	uint8_t mtlrx[2*1024+1];
	uint16_t tnum;
	uint8_t mtltx[2*1024+1];
} DWGMACState;
bool f8_is_full(DWGMACState *s,uint8_t f)
{
	uint16_t ret;
    ret= (f)?s->rnum:s->tnum;
    return (ret==2*1024);
}
bool f8_is_empty(DWGMACState *s,uint8_t f)
{
	uint16_t ret;
	ret= (f)?s->rnum:s->tnum;
	return (ret==0);
}
uint16_t f8_num_used(DWGMACState *s,uint8_t f)
{
	uint16_t ret;
	ret= (f)?s->rnum:s->tnum;
    return ret;
}

void f8_push(DWGMACState *s,uint8_t f, uint8_t data)
{
	uint16_t ret;
	ret= (f)?s->rnum:s->tnum;
    if (ret == 2*1024) {
        abort();
    }
    if (f){
    	s->rnum++;
    	s->mtlrx[s->rnum]=data;
    }else{
    	s->tnum++;
    	s->mtltx[s->tnum]=data;
    }
}
uint8_t f8_pop(DWGMACState *s,uint8_t f)
{
	uint8_t ret8;
	uint16_t ret,i;
	ret= (f)?s->rnum:s->tnum;
	if (ret== 0) {
		abort();
	}
	if(f){
		ret8= s->mtlrx[1] ;
		s->rnum--;
		for(i=2;i<=ret;i++){
		s->mtlrx[i-1]=s->mtlrx[i];
		}
	}else{
		ret8= s->mtltx[1] ;
		s->tnum--;
		for(i=2;i<=ret;i++){
		s->mtltx[i-1]=s->mtltx[i];
		}
	}

	return ret8;
}
static void dwmac_update_irq(DWGMACState *s)
{
	if (s->status&TI_MA ||s->status&TU_MA||s->status&RI_MA||s->status&ERI_MA)
		s->status|=NIS_MA;
	if (s->status&TPS_MA ||s->status&OVF_MA||s->status&UNF_MA||s->status&RU_MA||s->status&RPS_MA||s->status&ETI_MA)
		s->status|=AIS_MA;
	if (s->status&RI_MA)
		s->status&=~ERI_MA;
	s->status&=s->intenable;
	qemu_set_irq(s->irq, (s->status)?1:0);

}

static void send_p(DWGMACState *s, const uint8_t fl, int size)
{
	int i=0;
	uint8_t hbuf[2*1024+1];

	if (s->conf & 8){
		while(i<size){
			hbuf[i]=f8_pop(s,fl);
			i++;
		}
		qemu_send_packet(qemu_get_queue(s->nic),hbuf , size);
	}
}
static void dma_run_hp(DWGMACState *s)
{
	uint8_t *temp,h[4],t[2*1024];
	uint32_t i,j,z;
	if((s->status&trastate(7))!=(trastate(1))){
		return 0;
	}
	temp=s->currhosttxdesc;
	cpu_physical_memory_read( temp, h, 4);
	if (h[3]&0x80 ){
		temp=s->currhosttxdesc+1;
		cpu_physical_memory_read( temp, h, 4);
		i=0,j=0;
		i|=h[1]&3,j|=h[0];
		i=i<<8;
		i|=j;
		s->currhosttxbuffaddr=s->currhosttxdesc+2;
		s->tnum=0;
		temp=s->currhosttxbuffaddr;
		cpu_physical_memory_read( temp, h, 4);
		for(j=0,z=0;z<4;z++){
			j=j<<8;
			j|=h[3-z];
		}
		cpu_physical_memory_read( j, t, i);

		j=0;
		while(j<i){
			f8_push(s,0,t[j]);

			j++;
			if(f8_is_full(s,0)){
				s->currhosttxbuffaddr=s->currhosttxbuffaddr+j;
				break;
			}
		}
		temp=s->currhosttxdesc;
		cpu_physical_memory_read( temp, h, 4);
		h[3]&=0x7f;
		cpu_physical_memory_write( temp, h, 4);
	}
	temp=s->currhosttxdesc+1;
	cpu_physical_memory_read( temp, h, 4);
	if(~(h[3]&0x40)){
		temp=s->currhosttxdesc+3;
		cpu_physical_memory_read(temp, h, 4);

		for(j=0,z=0;z<4;z++){
			j=j<<8;
			j|=h[3-z];
		}
		s->currhosttxdesc=j;

	}
	s->status|=ETI_MA;
	send_p(s,0,f8_num_used(s,0));
	s->status|=TI_MA;
}
static void dma_run_ph(DWGMACState *s)
{
	uint8_t *temp,h[4],t[2*1024],temflag=0;
	uint32_t i,j,z;
	if((s->status&recestate(7))!=(recestate(1))){
		return 0;
	}
	temp=s->currhostrxdesc;
	cpu_physical_memory_read( temp, h, 4);
	if (h[3]&0x80 ){

		i=0,j=0;
		temp=s->currhostrxdesc;
		cpu_physical_memory_read( temp, h, 4);
		i=f8_num_used(s,1);
		h[2]=i&0xff;
		h[3]=(h[3]&0xc0)|((i&0x3f00)>>8);

		cpu_physical_memory_write( temp, h, 4);
		s->currhostrxbuffaddr=s->currhostrxdesc+2;
		temp=s->currhostrxbuffaddr;
		cpu_physical_memory_read( temp, h, 4);
		for(j=0,z=0;z<4;z++){
			j=j<<8;
			j|=h[3-z];
		}
		z=0;
		while(z<i){
			t[z]=f8_pop(s,1);
			z++;
			if(f8_is_empty(s,1)){
				s->currhostrxbuffaddr=s->currhostrxbuffaddr+z;
				break;
			}
		}
		cpu_physical_memory_write( j, t, i);
		temp=s->currhostrxdesc;
		cpu_physical_memory_read( temp, h, 4);
		h[3]&=0x7f;
		cpu_physical_memory_write( temp, h, 4);
	}else{
		s->status&=~recestate(7);
		s->status|=recestate(4);
		s->status|=RU_MA;
		temflag=1;
	}
	temp=s->currhostrxdesc+1;
	cpu_physical_memory_read( temp, h, 4);

	temp=s->currhostrxdesc+3;
	cpu_physical_memory_read(temp, h, 4);

	for(j=0,z=0;z<4;z++){
		j=j<<8;
		j|=h[3-z];
	}s->currhostrxdesc=j;

	if (!temflag){
		s->status|=RI_MA;
	}
}

static int dwmac_buffer_full(DWGMACState *s)
{
    int avail, index, boundary;
    if(f8_is_full(s,1))
    	return 1;
    return 0;
}

static int dwmac_can_receive(NetClientState *nc)
{
    DWGMACState *s = qemu_get_nic_opaque(nc);

    if (s->conf & 4)//maybe this need to be changed to machine stop
        return 1;
    return !dwmac_buffer_full(s);
}

#define MIN_BUF_SIZE 60

ssize_t dwmac_receive(NetClientState *nc, const uint8_t *buf, size_t size_)
{
	DWGMACState *s = qemu_get_nic_opaque(nc);
	int size = size_;
	uint8_t *p;
	unsigned int total_len, next, avail, len, index, mcast_idx;
	uint8_t buf1[60];
	static const uint8_t broadcast_macaddr[6] =
	{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

	if (!(s->conf &4) || dwmac_buffer_full(s))// can not receive
		return -1;
	if (s->framefilt & (1<<31)) {/* receive all */}
	else {

		if (!memcmp(buf,  broadcast_macaddr, 6)) {
			/* broadcast address */
			if (s->framefilt & 0x20)
				return size;
		} else if (buf[0] & 0x01) {
			/* multicast */
			if (!(s->framefilt & 0x10))
				return size;
		} else if ((s->macaddr0lo & 0x0ff) == buf[0] &&
				((s->macaddr0lo>>8) & 0x0ff) == buf[1] &&
				((s->macaddr0lo>>16) & 0x0ff )== buf[2] &&
				((s->macaddr0lo>>24)& 0x0ff) == buf[3] &&
				(s->macaddr0hi & 0x0ff)== buf[4] &&
				((s->macaddr0hi>>8)& 0x0ff)== buf[5]) {
			/* match */
			}
		else {
			if(s->framefilt&(1<<3)){
				;//maybe nothing
				}
			else{
				return size;
			}
		}
	}
	while(size){
		f8_push(s,1,*buf);
		buf++;
		if(f8_num_used(s,1)==2*1024){
			s->status|=OVF_MA;
			break;
		}
		size--;
	}

	s->status|=ERI_MA;
	dma_run_ph(s);
	dwmac_update_irq(s);
    return size_;
}

static void dwmac_write(void *opaque, uint32_t addr, uint32_t val)
{

	DWGMACState *s = opaque;
	uint8_t *temp,h[4];
#ifdef DEBUG_NE2000
    fprintf(stderr,"qemuNE2000: write addr=0x%x val=0x%x\n", addr, val);
#endif
	switch(addr) {
	case 0: /*mac register0*/
		s->conf=val;
		break;
	case 4:
		s->framefilt=val;
		break;
	//reserved case 8:
		s->hashtablehigh=val;
		break;
	//reserved case 0xc:
		s->hashtablelow=val;
		break;
	case 0x10:// directly send packet and no phy
		s->miiaddr=val;
		s->miiaddr&=0xfffe;
		s->miidata=0x55555555;
		break;
	case 0x14:// directly send packet and no phy
		s->miidata=val;
		break;
	case 0x18:
		s->flowcontrol=val;
		break;
	case 0x1c:
		s->vlantag=val;
		break;
	//RO case 0x20:
		s->version=val;
		break;
	case 0x38:
		s->intreg=val;
		break;
	case 0x3c:
		s->intmask=val;
		break;
	case 0x40:
		s->macaddr0hi=val;
		break;
	case 0x44:
		s->macaddr0lo=val;
		break;





	case 0x1000: /*dma register0*/
		s->busmode=val;
		if (val&1)
			dwmac_reset(s);
		break;
	case 0x1004:
		s->txpolldemand=val;
		temp=s->currhosttxdesc;
		cpu_physical_memory_read( temp, h, 4);
		if(!(h[3]&0x80)){
			s->status&=~trastate(7);
			s->status|=trastate(4);
			s->status|=TU_MA;
		}else{
			s->status&=~trastate(7);
			s->status|=trastate(1);
			dma_run_hp(s);
		}
		break;
	case 0x1008:
		s->rxpolldemand=val;
		temp=s->currhosttxdesc;
		cpu_physical_memory_read( temp, h, 4);
		if(!!(h[3]&0x80)){
			s->status&=~recestate(7);
			s->status|=recestate(4);
			s->status|=RU_MA;
		}else{
			s->status&=~recestate(7);
			s->status|=recestate(1);
			dma_run_ph(s);
		}
		break;
	case 0x100c:
		s->rxdesclistaddr=val;
		s->currhostrxdesc=s->rxdesclistaddr;
		break;
	case 0x1010:
		s->txdesclistaddr=val;
		s->currhosttxdesc=s->txdesclistaddr;
		break;
	case 0x1014:
		s->status&=~val;
		break;
	case 0x1018:
		s->opmode=val;

		if(val&(1<<20)){
			s->mtltx[0]=0;
			s->opmode&=~(1<<20);
		}
		if((val&(1<<21))&&(s->mtltx[0])){
			send_p(s,0,f8_num_used(s,0));
			if(f8_num_used(s,0)<0)
				s->status|=UNF_MA;
			s->status|=TI_MA;
			dwmac_update_irq(s);
			s->opmode&=~(1<<21);
		}
		if(val&(1<<13)){
			s->status&=~trastate(7);
			s->status|=trastate(1);
		}
		if(val&(1<<1)){
			s->status&=~recestate(7);
			s->status|=recestate(1);
		}
		break;
	case 0x101c:
		s->intenable=val;
		break;
	case 0x1028:
		s->axibus=val;
		break;
	case 0x1048:
		s->currhosttxdesc=val;
		break;
	case 0x104c:
		s->currhostrxdesc=val;
		break;
	case 0x1050:
		s->currhosttxbuffaddr=val;
		break;
	case 0x1054:
		s->currhostrxbuffaddr=val;
		break;
	}
	return 0;
}

static uint32_t dwmac_read(void *opaque, uint32_t addr)
{
    DWGMACState *s = opaque;
    int ret;
    switch(addr) {
    case 0: /*mac register0*/
    	ret = s->conf;
    	break;
    case 4:
    	ret = s->framefilt;
    	break;
    //reserved case 8:
    	ret = s->hashtablehigh;
    	break;
    //reserved case 0xc:
    	ret = s->hashtablelow;
    	break;
    case 0x10:// directly send packet and no phy
    	ret = s->miiaddr;
    	break;
    case 0x14:// directly send packet and no phy
    	ret = s->miidata&0x7fff;
    	break;
    case 0x18:
    	ret = s->flowcontrol;
    	break;
    case 0x1c:
    	ret = s->vlantag;
    	break;
    case 0x20:
    	ret = s->version;
    	break;
    case 0x38:
    	ret = s->intreg;
    	break;
    case 0x3c:
    	ret = s->intmask;
    	break;
    case 0x40:
    	ret = s->macaddr0hi;
    	break;
    case 0x44:
    	ret = s->macaddr0lo;
    	break;



    case 0x1000: /*dma register0*/
    	ret = s->busmode;
    	break;
    case 0x1004:
    	ret = 0;
    	break;
    case 0x1008:
    	ret = s->rxpolldemand;
    	break;
    case 0x100c:
    	ret = s->rxdesclistaddr;
    	break;
    case 0x1010:
    	ret = s->txdesclistaddr;
    	break;
    case 0x1014:
    	ret = s->status;
    	break;
    case 0x1018:
    	ret = s->opmode;
    	break;
    case 0x101c:
    	ret = s->intenable;
    	break;
    case 0x1028:
    	ret = s->axibus;
    	break;
    case 0x1048:
    	ret = s->currhosttxdesc;
    	break;
    case 0x104c:
    	ret = s->currhostrxdesc;
    	break;
    case 0x1050:
    	ret = s->currhosttxbuffaddr;
    	break;
    case 0x1054:
    	ret = s->currhostrxbuffaddr;
    	break;
    }
    #ifdef DEBUG_dwmac
        fprintf(stderr,"qemudwmac: read addr=0x%x val=%x\n", addr, ret);
    #endif
    return ret;
}

const VMStateDescription vmstate_dwmac = {
    .name = "dwmac",
    .version_id = 2,
    .minimum_version_id = 0,
    .fields = (VMStateField[]) {
        VMSTATE_UINT32(conf, DWGMACState),
        VMSTATE_UINT32(framefilt, DWGMACState),
        VMSTATE_UINT32(hashtablehigh, DWGMACState),
        VMSTATE_UINT32(hashtablelow, DWGMACState),
        VMSTATE_UINT32(miiaddr, DWGMACState),
        VMSTATE_UINT32(miidata, DWGMACState),
        VMSTATE_UINT32(flowcontrol, DWGMACState),
        VMSTATE_UINT32(vlantag, DWGMACState),
        VMSTATE_UINT32(version, DWGMACState),
        VMSTATE_UINT32(intreg, DWGMACState),
        VMSTATE_UINT32(intmask, DWGMACState),
        VMSTATE_UINT32(macaddr0hi, DWGMACState),
        VMSTATE_UINT32(macaddr0lo, DWGMACState),
        VMSTATE_UINT32(busmode, DWGMACState),
        VMSTATE_UINT32(txpolldemand, DWGMACState),
        VMSTATE_UINT32(rxpolldemand, DWGMACState),
        //VMSTATE_UINT32(rxdesclistaddr, DWGMACState),
       // VMSTATE_UINT32(txdesclistaddr, DWGMACState),
        VMSTATE_UINT32(status, DWGMACState),
        VMSTATE_UINT32(opmode, DWGMACState),
        VMSTATE_UINT32(intenable, DWGMACState),
        VMSTATE_UINT32(axibus, DWGMACState),
        //VMSTATE_UINT32(currhosttxdesc, DWGMACState),
        //VMSTATE_UINT32(currhostrxdesc, DWGMACState),
        //VMSTATE_UINT32(currhosttxbuffaddr, DWGMACState),
        //VMSTATE_UINT32(currhostrxbuffaddr, DWGMACState),
		VMSTATE_BUFFER(mtlrx, DWGMACState),
		VMSTATE_BUFFER(mtltx, DWGMACState),
        VMSTATE_END_OF_LIST()
    }
};

static const MemoryRegionOps dwmac_ops = {
    .read = dwmac_read,
    .write = dwmac_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
};

static NetClientInfo dwmacnc_info = {
    .type = NET_CLIENT_OPTIONS_KIND_NIC,
    .size = sizeof(NICState),
    .can_receive = dwmac_can_receive,
    .receive = dwmac_receive,
};
void dwmac_reset(DWGMACState *s)
{
    int i;
    s->conf=0;
    s->framefilt=0;
    s->hashtablehigh=0;
    s->hashtablelow=0;
    s->miiaddr=0;
    s->miidata=0;
	s->flowcontrol=0;
	s->vlantag=0;
	s->version=0x037;
	s->intreg=0;
	s->intmask=0;
	s->macaddr0hi=0xffff;
	s->macaddr0lo=0xfffe;

	s->busmode=0;
	s->txpolldemand=0;
	s->rxpolldemand=0;
	s->rxdesclistaddr=0;
	s->txdesclistaddr=0;
	s->status=0;
	s->opmode=0;
	s->intenable=0xffffffff;
	s->axibus=0;
	s->currhosttxdesc=0;
	s->currhostrxdesc=0;
	s->currhosttxbuffaddr=0;
	s->currhostrxbuffaddr=0;

}

static void dwmac_instance_init(SysBusDevice *sbd)
{
	DeviceState *dev = DEVICE(sbd);
	DWGMACState *s = DWGMAC(dev);

    memory_region_init_io(&s->iomem, OBJECT(s), &dwmac_ops, s, TYPE_DWGMAC, 0x2000);
    sysbus_init_mmio(sbd, &s->iomem);
    sysbus_init_irq(sbd, &s->irq);
    qdev_set_nic_properties(dev, &nd_table[0]);

    s->c.peers.ncs[0]=nd_table[0].netdev;
    s->nic = qemu_new_nic(&dwmacnc_info, &s->c,
                              object_get_typename(OBJECT(s)), TYPE_DWGMAC, s);
    qemu_format_nic_info_str(qemu_get_queue(s->nic), s->c.macaddr.a);
    nd_table[0].netdev=s->nic->ncs;
    dwmac_reset(s);
    vmstate_register(dev, -1, &vmstate_dwmac, s);


}

static Property dwmac_properties[] = {
    DEFINE_NIC_PROPERTIES(DWGMACState, c),
    DEFINE_PROP_END_OF_LIST(),
};

static void dwmac_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    dc->init=dwmac_instance_init;

    dc->props = dwmac_properties;
    set_bit(DEVICE_CATEGORY_NETWORK, dc->categories);
}

static const TypeInfo dwmac_info = {
    .name          = TYPE_DWGMAC,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(DWGMACState),
    .class_init    = dwmac_class_init,
};
static void dwmac_register_types(void)
{
    type_register_static(&dwmac_info);
}

type_init(dwmac_register_types)

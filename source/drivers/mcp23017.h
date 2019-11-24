#ifndef DRIVERS_MCP23017_H_
#define DRIVERS_MCP23017_H_

#include "stdint.h"
#include "stdbool.h"

/* ----------------------------------------------------------------------------- */
/* ---------------------------------- MACROS ----------------------------------- */
/* ----------------------------------------------------------------------------- */

/* I2C addresses */
#define MCP23017_MAX_DEVICES	8
#define MCP23017_PORT_PINS		8
#define MCP23017_BASE_ADDR		0x20
#define MCP23017_DEV0			0x20
#define MCP23017_DEV1			0x21
#define MCP23017_DEV2			0x22
#define MCP23017_DEV3			0x23
#define MCP23017_DEV4			0x24
#define MCP23017_DEV5			0x25
#define MCP23017_DEV6			0x26
#define MCP23017_DEV7			0x27

/* Set this macro to use BANK1 registers map rather than BANK0 */
#define MCP23017_REG_BANK1 0

/* Bit macros */
#define MCP23017_UINT8_BIT(BIT)             ((uint8_t)(1 << (BIT)))
#define MCP23017_UINT8_CBIT(BIT)            ((uint8_t)~(1 << (BIT)))
#define MCP23017_UINT8_BIT_SET(V, BIT)      {(V) = (uint8_t)((V) | MCP23017_UINT8_BIT((BIT)));}
#define MCP23017_UINT8_BIT_CLEAR(V, BIT)    {(V) = (uint8_t)((V) & MCP23017_UINT8_CBIT((BIT)));}
#define MCP23017_UINT8_IS_BIT_SET(V, BIT)   (((uint8_t)(V) & MCP23017_UINT8_BIT((BIT))) != 0x00)
#define MCP23017_UINT8_GET_BIT(V, BIT)		((uint8_t)(V) >> (BIT) & 0x01)
#define MCP23017_UINT8_TOGGLE_BIT(V, BIT)	{(V) = (uint8_t)((V) ^ MCP23017_UINT8_BIT((BIT)));}
#define MCP23017_UINT8_TOGGLE_BITS(V)		(~((uint8_t)(V)))

/* Registers */
#if defined(MCP23017_REG_BANK1) && MCP23017_REG_BANK1
#define MCP23017_IODIRA     0x00
#define MCP23017_IODIRB     0x10
#define MCP23017_IPOLA      0x01
#define MCP23017_IPOLB      0x11
#define MCP23017_GPINTENA   0x02
#define MCP23017_GPINTENB   0x12
#define MCP23017_DEFVALA    0x03
#define MCP23017_DEFVALB    0x13
#define MCP23017_INTCONA    0x04
#define MCP23017_INTCONB    0x14
#define MCP23017_IOCON      0x05
#define MCP23017_GPPUA      0x06
#define MCP23017_GPPUB      0x16
#define MCP23017_INTFA      0x07
#define MCP23017_INTFB      0x17
#define MCP23017_INTCAPA    0x08
#define MCP23017_INTCAPB    0x18
#define MCP23017_GPIOA      0x09
#define MCP23017_GPIOB      0x19
#define MCP23017_OLATA      0x0A
#define MCP23017_OLATB      0x1A
#else
#define MCP23017_IODIRA     0x00
#define MCP23017_IODIRB     0x01
#define MCP23017_IPOLA      0x02
#define MCP23017_IPOLB      0x03
#define MCP23017_GPINTENA   0x04
#define MCP23017_GPINTENB   0x05
#define MCP23017_DEFVALA    0x06
#define MCP23017_DEFVALB    0x07
#define MCP23017_INTCONA    0x08
#define MCP23017_INTCONB    0x09
#define MCP23017_IOCON      0x0A
#define MCP23017_GPPUA      0x0C
#define MCP23017_GPPUB      0x0D
#define MCP23017_INTFA      0x0E
#define MCP23017_INTFB      0x0F
#define MCP23017_INTCAPA    0x10
#define MCP23017_INTCAPB    0x11
#define MCP23017_GPIOA      0x12
#define MCP23017_GPIOB      0x13
#define MCP23017_OLATA      0x14
#define MCP23017_OLATB      0x15
#endif

/* IOCON bits */
#define MCP23017_IOCON_INTPOL_POS   0x01
#define MCP23017_IOCON_INTPOL_MSK   MCP23017_UINT8_BIT(MCP23017_IOCON_INTPOL_POS)
#define MCP23017_IOCON_ODR_POS      0x02
#define MCP23017_IOCON_ODR_MSK      MCP23017_UINT8_BIT(MCP23017_IOCON_ODR_POS)
#define MCP23017_IOCON_HAEN_POS     0x03
#define MCP23017_IOCON_HAEN_MSK     MCP23017_UINT8_BIT(MCP23017_IOCON_HAEN_POS) /* MCP23017 does not recognize this bit. Address pins are always enabled */
#define MCP23017_IOCON_DISSLW_POS   0x04
#define MCP23017_IOCON_DISSLW_MSK   MCP23017_UINT8_BIT(MCP23017_IOCON_DISSLW_POS)
#define MCP23017_IOCON_SEQOP_POS    0x05
#define MCP23017_IOCON_SEQOP_MSK    MCP23017_UINT8_BIT(MCP23017_IOCON_SEQOP_POS)
#define MCP23017_IOCON_MIRROR_POS   0x06
#define MCP23017_IOCON_MIRROR_MSK   MCP23017_UINT8_BIT(MCP23017_IOCON_MIRROR_POS)
#define MCP23017_IOCON_BANK_POS     0x07
#define MCP23017_IOCON_BANK_MSK     MCP23017_UINT8_BIT(MCP23017_IOCON_BANK_POS)

/* Predefined constants */
#define MCP23017_INPUT              0x01    /* Pin as input */
#define MCP23017_OUTPUT             0x00    /* Pin as output */
#define MCP23017_INV                0x01    /* Input inverted */
#define MCP23017_NOT_INV            0x00    /* Input not inverted */
#define MCP23017_INT_EN             0x01    /* Pin interrupt enabled */
#define MCP23017_INT_DIS            0x00    /* Pin interrupt disabled */
#define MCP23017_INT_COMP_DEFVAL    0x01    /* Pin value is compared against the DEFVAL register value */
#define MCP23017_INT_COM_PREV       0x00    /* Pin value is compared against the previous state */
#define MCP23017_BANK0              0x00    /* BANK0 registers map is used */
#define MCP23017_BANK1              0x01    /* BANK1 registers map is used */
#define MCP23017_INT_MIRROR_EN      0x01    /* Interrupt pins are internally connected */
#define MCP23017_INT_MIRROR_DIS     0x00    /* Interrupt pins are not connected */
#define MCP23017_SEQ_OP_EN          0x01    /* Sequential operation enabled */
#define MCP23017_SEQ_OP_DIS         0x00    /* Sequential operation disabled */
#define MCP23017_SLEW_RATE_EN       0x01    /* Slew rate on */
#define MCP23017_SLEW_RATE_DIS      0x00    /* Slew rate off */
#define MCP23017_ADDR_PINS_EN       0x01    /* Address pins enabled */
#define MCP23017_ADDR_PINS_DIS      0x00    /* Address pins disabled */
#define MCP23017_OPEN_DRAIN_EN      0x01    /* Open-drain output */
#define MCP23017_OPEN_DRAIN_DIS     0x00    /* Active driver output */
#define MCP23017_INT_POL_HIGH       0x01    /* INT pins active high */
#define MCP23017_INT_POL_LOW        0x00    /* INT pins active low */
#define MCP23017_PULLUP_EN          0x01    /* Pull-up resistors enabled */
#define MCP23017_PULLUP_DIS         0x00    /* Pull-up resistors disabled */
#define MCP23017_PIN_LOW            0x00    /* Pin state low */
#define MCP23017_PIN_HIGH           0x01    /* Pin state high */

/* Preset IOCON settings */
#define MCP23017_IOCON_PRESET		(MCP23017_UINT8_BIT(MCP23017_IOCON_INTPOL_POS) | MCP23017_UINT8_BIT(MCP23017_IOCON_MIRROR_POS))

/* Macro for choosing correct port register (A or B) */
#define MCP23017_GET_REG_ADDR(PORT, REG_A, REG_B) ((PORT) == MCP23017_PORT_A ? (REG_A) : (REG_B))

/* ----------------------------------------------------------------------------- */
/* ---------------------------------- DATA TYPES ------------------------------- */
/* ----------------------------------------------------------------------------- */

/*!
 * \brief MCP23017 chain information
 */
typedef struct {
	uint8_t chainId;		//!< Chain ID. Can be used to distinguish between chains
	uint8_t numDevices;		//!< Number of devices used (1-8)
	uint8_t chainBaseAddr;	//!< I2C address of first device in the chain
} MCP23017_Chain_t;

/*!
 * \brief Send function header
 *
 * \param devAddr : Device address
 * \param regAddr : Register address
 * \param txData  : Register data
 * \return True if transmission succeed, false otherwise
 */
typedef bool (*MCP23017_SendFn_t)(uint8_t devAddr, uint8_t regAddr, uint8_t txData);

/*!
 * \brief Receive function header
 *
 * \param devAddr : Device address
 * \param regAddr : Register address
 * \param rxData  : Receive buffer
 * \return True if transmission succeed, false otherwise
 */
typedef bool (*MCP23017_ReceiveFn_t)(uint8_t devAddr, uint8_t regAddr, uint8_t *rxData);

/*!
 * \brief IOCON register format
 *
 * \note Format: BANK(7) | MIRROR(6) | SEQOP(5) | DISSLW(4) | HAEN(3) | ODR(2) | INTPOL(1) | UNIMPLEMENTED(0)
 */
typedef uint8_t MCP23017_IOCON_t;

/*!
 * Return codes used in library functions
 */
typedef enum {
	MCP23017_SUCCESS,		//!< Everything OK
	MCP23017_MISSING_FN,	//!< Send/receive function not passed
	MCP23017_SENDING_ERR,	//!< I2C sending error (typically NAK was returned)
	MCP23017_RECEIVING_ERR	//!< I2C receiving error
} MCP23017_StatusCode_t;

/*!
 * \brief On-chip ports
 */
typedef enum {
	MCP23017_PORT_A = MCP23017_GPIOA,	//!< Port A
	MCP23017_PORT_B = MCP23017_GPIOB 	//!< Port B
} MCP23017_Port_t;

/* ----------------------------------------------------------------------------- */
/* ------------------------------- PUBLIC FUNCTIONS ---------------------------- */
/* ----------------------------------------------------------------------------- */

/*!
 * \brief Generic MCP23017 send data function. Although it can be surely used it is better to use dedicated api functions
 *
 * \param chain : Chain base address
 * \param devNum : Device number in the chain (0-7)
 * \param regAddr : Register address
 * \param regData : Register data
 * \return Instance of MCP23017_StatusCode_t
 */
MCP23017_StatusCode_t MCP23017_SendGeneric(const MCP23017_Chain_t *chain, uint8_t devNum, uint8_t regAddr, uint8_t regData);

/*!
 * \brief Generic MCP23017 receive data function. Although it can be surely used it is better to use dedicated api functions
 *
 * \param chain : Chain base address
 * \param devNum : Device number in the chain (0-7)
 * \param regAddr : Register address
 * \param dst : Receive buffer base address
 * \return Instance of MCP23017_StatusCode_t
 */
MCP23017_StatusCode_t MCP23017_ReceiveGeneric(const MCP23017_Chain_t *chain, uint8_t devNum, uint8_t regAddr, uint8_t *dst);

/*!
 * \brief Init MCP23017 by sending I2C read/write functions
 *
 * \param sendFn : Send function base address
 * \param receiveFn : Receive function base address
 */
void MCP23017_Init(MCP23017_SendFn_t sendFn, MCP23017_ReceiveFn_t receiveFn);

/*!
 * \brief Initialize MCP23017 devices chain
 *
 * \param chain : Chain base address
 * \param ioconf : IOCON register settings
 * \return Instance of MCP23017_StatusCode_t
 * \note When BANK1 registers map is used the BANK1 bit in IOCON register will be set even if user did not set it
 */
MCP23017_StatusCode_t MCP23017_ChainInit(const MCP23017_Chain_t *chain, MCP23017_IOCON_t ioconf);

/*!
 * \brief Set port direction (input/output)
 *
 * \param chain : Chain base address
 * \param devNum : Device number in the chain (0-7)
 * \param port : See MCP23017_Port_t for more information
 * \param portDirection : Bit 0 reflects the direction of pin 0, ..., bit 7 reflects the state of pin 7. Bit set = input, bit cleared = output
 * \return Instance of MCP23017_StatusCode_t
 */
static inline MCP23017_StatusCode_t MCP23017_PortSetDirection(const MCP23017_Chain_t *chain, uint8_t devNum, MCP23017_Port_t port, uint8_t portDirection)
{
	return MCP23017_SendGeneric(chain, devNum, MCP23017_GET_REG_ADDR(port, MCP23017_IODIRA, MCP23017_IODIRB), portDirection);
}

/*!
 * \brief Set pin logic state
 *
 * \param chain : Chain base address
 * \param devNum : Device number in the chain (0-7)
 * \param port : See MCP23017_Port_t for more information
 * \param pin : Desired pin (0-7)
 * \param value : Logic value
 * \return Instance of MCP23017_StatusCode_t
 */
MCP23017_StatusCode_t MCP23017_PinWrite(const MCP23017_Chain_t *chain, uint8_t devNum, MCP23017_Port_t port, uint8_t pin, bool value);


/*!
 * \brief Set port logic state
 *
 * \param chain : Chain base address
 * \param devNum : Device number in the chain (0-7)
 * \param port : See MCP23017_Port_t for more information
 * \param states : Bit 0 reflects the state of pin 0, ..., bit 7 reflects the state of pin 7. Bit set = logic high, bit cleared = logic low
 * \return Instance of MCP23017_StatusCode_t
 */
static inline MCP23017_StatusCode_t MCP23017_PortWrite(const MCP23017_Chain_t *chain, uint8_t devNum, MCP23017_Port_t port, uint8_t states)
{
	return MCP23017_SendGeneric(chain, devNum, port, states);
}

/*!
 * \brief Read port logic state
 *
 * \param chain : Chain base address
 * \param devNum : Device number in the chain (0-7)
 * \param port : See MCP23017_Port_t for more information
 * \param dst : Destination buffer where result will be stored
 * \return Instance of MCP23017_StatusCode_t
 */
static inline MCP23017_StatusCode_t MCP23017_PortRead(const MCP23017_Chain_t *chain, uint8_t devNum, MCP23017_Port_t port, uint8_t *dst)
{
	return MCP23017_ReceiveGeneric(chain, devNum, port, dst);
}

/*!
 * \brief Read pin logic state
 *
 * \param chain : Chain base address
 * \param devNum : Device number in the chain (0-7)
 * \param port : See MCP23017_Port_t for more information
 * \param pin : Desired pin (0-7)
 * \param dst : Destination buffer where result will be stored
 * \return Instance of MCP23017_StatusCode_t
 */
MCP23017_StatusCode_t MCP23017_PinRead(const MCP23017_Chain_t *chain, uint8_t devNum, MCP23017_Port_t port, uint8_t pin, bool *dst);

/*!
 * \brief Toggle pin logic state
 *
 * \param chain : Chain base address
 * \param devNum : Device number in the chain (0-7)
 * \param port : See MCP23017_Port_t for more information
 * \param pin : Desired pin (0-7)
 * \return Instance of MCP23017_StatusCode_t
 */
MCP23017_StatusCode_t MCP23017_PinToggle(const MCP23017_Chain_t *chain, uint8_t devNum, MCP23017_Port_t port, uint8_t pin);

/*!
 * \brief Enable port interrupts
 *
 * \param chain : Chain base address
 * \param devNum : Device number in the chain (0-7)
 * \param port : See MCP23017_Port_t for more information
 * \param intCtrl : Bit 0 reflects the interrupt control of pin 0, ..., bit 7 reflects the state of pin 7. Bit set = interrupt enabled, bit cleared = interrupt disabled
 * \return Instance of MCP23017_StatusCode_t
 */
static inline MCP23017_StatusCode_t MCP23017_PortEnableInt(const MCP23017_Chain_t *chain, uint8_t devNum, MCP23017_Port_t port, uint8_t intCtrl)
{
	return MCP23017_SendGeneric(chain, devNum, MCP23017_GET_REG_ADDR(port, MCP23017_GPINTENA, MCP23017_GPINTENB), intCtrl);
}

/*!
 * \brief Set port DEFVAL values
 *
 * \param chain : Chain base address
 * \param devNum : Device number in the chain (0-7)
 * \param port : See MCP23017_Port_t for more information
 * \param defval : Comparison bits
 * \return Instance of MCP23017_StatusCode_t
 */
static inline MCP23017_StatusCode_t MCP23017_PortSetIntDefval(const MCP23017_Chain_t *chain, uint8_t devNum, MCP23017_Port_t port, uint8_t defval)
{
	return MCP23017_SendGeneric(chain, devNum, MCP23017_GET_REG_ADDR(port, MCP23017_DEFVALA, MCP23017_DEFVALB), defval);
}

/*!
 * \brief Set port interrupt mode
 *
 * \param chain : Chain base address
 * \param devNum : Device number in the chain (0-7)
 * \param port : See MCP23017_Port_t for more information
 * \param mode : Bit 0 reflects the interrupt mode control of pin 0, ..., bit 7 reflects the state of pin 7. Bit set = defval interrupt, bit cleared = prev state interrupt
 * \return Instance of MCP23017_StatusCode_t
 */
static inline MCP23017_StatusCode_t MCP23017_PortSetIntMode(const MCP23017_Chain_t *chain, uint8_t devNum, MCP23017_Port_t port, uint8_t mode)
{
	return MCP23017_SendGeneric(chain, devNum, MCP23017_GET_REG_ADDR(port, MCP23017_INTCONA, MCP23017_INTCONA), mode);
}

/*!
 * \brief Clear port interrupt status
 *
 * \param chain : Chain base address
 * \param devNum : Device number in the chain (0-7)
 * \param port : See MCP23017_Port_t for more information
 * \param intCap : If true INTCAP register value will be returned, if false GPIO register will be returned
 * \param dst : Destination buffer where port logic status will be stored
 * \return Instance of MCP23017_StatusCode_t
 */
MCP23017_StatusCode_t MCP23017_PortClearInt(const MCP23017_Chain_t *chain, uint8_t devNum, MCP23017_Port_t port, bool intCap, uint8_t *dst);

/*!
 * \brief Enable port pullup
 *
 * \param chain : Chain base address
 * \param devNum : Device number in the chain (0-7)
 * \param port : See MCP23017_Port_t for more information
 * \param mode : Bit 0 reflects the pullup mode of pin 0, ..., bit 7 reflects the state of pin 7. Bit set = pullup enabled, bit cleared = pullup disabled
 * \return Instance of MCP23017_StatusCode_t
 */
static inline MCP23017_StatusCode_t MCP23017_PortSetInputPullUpMode(const MCP23017_Chain_t *chain, uint8_t devNum, MCP23017_Port_t port, uint8_t mode)
{
	return MCP23017_SendGeneric(chain, devNum, MCP23017_GET_REG_ADDR(port, MCP23017_GPPUA, MCP23017_GPPUB), mode);
}

/*!
 * \brief Set port input polarity
 *
 * \param chain : Chain base address
 * \param devNum : Device number in the chain (0-7)
 * \param port : See MCP23017_Port_t for more information
 * \param polarity : Bit 0 reflects the polarity of pin 0, ..., bit 7 reflects the state of pin 7. Bit set = polarity inverted, bit cleared = polarity not inverted
 * \return
 */
static inline MCP23017_StatusCode_t MCP23017_PortSetInputPolarity(const MCP23017_Chain_t *chain, uint8_t devNum, MCP23017_Port_t port, uint8_t polarity)
{
	return MCP23017_SendGeneric(chain, devNum, MCP23017_GET_REG_ADDR(port, MCP23017_IPOLA, MCP23017_IPOLB), polarity);
}

#endif /* DRIVERS_MCP23017_H_ */

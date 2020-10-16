#ifndef MEDICINA_C_BAND_DEFS
#define MEDICINA_C_BAND_DEFS

/* ** RECEIVER CONFIGURATION ** */

#define CONF_CCC_Normal_str "CCCNormalSetup" /**< CCC normal setup mnemonic string */
#define CONF_CCC_Narrow_str "CCCNarrowSetup" /**< CCC narrow setup mnemonic string */

#define CONF_CHC_Normal_str "CHCNormalSetup" /**< CHC normal setup mnemonic string */
#define CONF_CHC_Narrow_str "CHCNarrowSetup" /**< CHC narrow setup mnemonic string */

/* ** DATA BLOCK CONFIGURATION PATH ** */

#define MED_C_BAND_CONFIG_PATH "DataBlock/MedicinaCBand"
#define SYNTH_1ST_TABLE_PATH CONFIG_PATH"/Synthesizer1"
#define SYNTH_2ND_TABLE_PATH CONFIG_PATH"/Synthesizer2"
#define FEEDTABLE_PATH CONFIG_PATH"/Feeds"
#define TAPERTABLE_PATH CONFIG_PATH"/Taper"

#endif
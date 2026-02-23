#include "IRATools.h"

#include <stdlib.h>

const double TOLERANCE = 1e-6;

namespace IRALibraryTest {

class IRALibrary_IRATools : public ::testing::Test {
public:
	::testing::AssertionResult makeDirectory_createSimpleDirectory() {
		IRA::CString fullPath;
		fullPath=basePath+simpleDirPath;
		RecordProperty("description","check if a single level folder can be created");
		if (IRA::CIRATools::makeDirectory(fullPath)) {
			if (IRA::CIRATools::directoryExists(fullPath)) {
				return ::testing::AssertionSuccess();
			}
			else {
				return ::testing::AssertionFailure() << (const char *) fullPath << " does not exists";
			}
		}
		else {
			return ::testing::AssertionFailure() << (const char *) fullPath << " creation failed";
		}
	}

	::testing::AssertionResult makeDirectory_createComplexDirectory() {
		IRA::CString fullPath;
		fullPath=basePath+complexDirPath;
		::testing::Test::RecordProperty("description","check if a double level folder can be created");
		if (IRA::CIRATools::makeDirectory(fullPath)) {
			if (IRA::CIRATools::directoryExists(fullPath)) {
				return ::testing::AssertionSuccess();
			}
			else {
				return ::testing::AssertionFailure() << (const char *) fullPath << " does not exists";
			}
		}
		else {
			return ::testing::AssertionFailure() << (const char *) fullPath << " creation failed";
		}
	}

	::testing::AssertionResult fileExists_checkExistance() {
		IRA::CString file;
		::testing::Test::RecordProperty("description","check if an existing file is detected");
		file=basePath+fileName;
		if (IRA::CIRATools::fileExists(file)) {
			return ::testing::AssertionSuccess();
		}
		else {
			return ::testing::AssertionFailure() << (const char *) file << " should be present but (fileExists) fails to detect it";
		}
	}

	::testing::AssertionResult fileExists_checkNoExistance() {
		IRA::CString file;
		::testing::Test::RecordProperty("description","check if a not existing file is correctly not detected");
		file="fooAnddymmyFile.test";
		if (IRA::CIRATools::fileExists(file)) {
			return ::testing::AssertionFailure() << (const char *) file << " should not be present but (fileExists) detects it";
		}
		else {
			return ::testing::AssertionSuccess();
		}
	}
	
	::testing::AssertionResult matchRegExp_checkCorrectMatch() {
		IRA::CString input("giant1spaghetti.monster doesnotmatch gino.pino white1black 1112121 giant1spaghetti.0122");
		IRA::CString reg("[A-Za-z0-1]+\\.[A-Za-z]+");
		std::vector<IRA::CString> res;
		::testing::Test::RecordProperty("description","check if parser correctly match the tokens");
		if (IRA::CIRATools::matchRegExp(input,reg,res)) {
			if (res.size()!=2) return ::testing::AssertionFailure() << " number of matched tokens should be 2";
			if (res[1]=="gino.pino") {
				return ::testing::AssertionSuccess();
			}
			else {
				return ::testing::AssertionFailure() << " number of matched tokens should be 2";
			}
		}
		else {
			return ::testing::AssertionFailure();
		}
	}
	
	void tuneLocalOscillator_checkSeveralCombinations() {
		double fs=-375;
		double fbw=275;
		double bs=0;
		double bbw=800;
	 	double lo=21000;
	 	double topo=22080;
	 	IRA::CString dev="LO";
		bool res=IRA::CIRATools::tuneLocalOscillator(fs,fbw,lo,bs,bbw,topo,dev);
		ASSERT_TRUE(res);
		EXPECT_NEAR(lo,22480.0,TOLERANCE);
		EXPECT_NEAR(bs,0.0,TOLERANCE);
		
		fs=-375;
		fbw=275;
		bs=0;
		bbw=1400;
	 	lo=21000;
	 	topo=22080;
	 	dev="ALL";
		res=IRA::CIRATools::tuneLocalOscillator(fs,fbw,lo,bs,bbw,topo,dev);
		ASSERT_FALSE(res) << "Backend center band is outside the IF boundaries";
		
		fs=-375;
		fbw=275;
		bs=0;
		bbw=1200;
	 	lo=21000;
	 	topo=22080;
	 	dev="ALL";
		res=IRA::CIRATools::tuneLocalOscillator(fs,fbw,lo,bs,bbw,topo,dev);
		ASSERT_TRUE(res);
		EXPECT_NEAR(lo,22680.0,TOLERANCE);
		EXPECT_NEAR(bs,0.0,TOLERANCE);
		
		fs=-375;
		fbw=800;
		bs=-100;
		bbw=1200;
	 	lo=21000;
	 	topo=22080;
	 	dev="LO";
		res=IRA::CIRATools::tuneLocalOscillator(fs,fbw,lo,bs,bbw,topo,dev);	
		ASSERT_TRUE(res);
		EXPECT_NEAR(lo,22780.0,TOLERANCE);
		EXPECT_NEAR(bs,-100.0,TOLERANCE);
		
		fs=-375;
		fbw=800;
		bs=-100;
		bbw=1200;
	 	lo=21000;
	 	topo=22080;
	 	dev="BCK";
		res=IRA::CIRATools::tuneLocalOscillator(fs,fbw,lo,bs,bbw,topo,dev);		
		ASSERT_FALSE(res) << "Target topocentric frequency could not be reached without moving the LO";	
		
		fs=-375;
		fbw=800;
		bs=-100;
		bbw=1200;
	 	lo=22800;
	 	topo=22080;
	 	dev="BCK";
		res=IRA::CIRATools::tuneLocalOscillator(fs,fbw,lo,bs,bbw,topo,dev);		
		ASSERT_TRUE(res);
		EXPECT_NEAR(lo,22800.0,TOLERANCE);
		EXPECT_NEAR(bs,-120.0,TOLERANCE);	
		
		fs=-375;
		fbw=800;
		bs=-100;
		bbw=1200;
	 	lo=22800;
	 	topo=22080;
	 	dev="ALL";
		res=IRA::CIRATools::tuneLocalOscillator(fs,fbw,lo,bs,bbw,topo,dev);	
		ASSERT_TRUE(res);
		EXPECT_NEAR(lo,22780.0,TOLERANCE);
		EXPECT_NEAR(bs,-100.0,TOLERANCE);		
		
		fs=-375;
		fbw=275;
		bs=400;
		bbw=64;
	 	lo=21000;
	 	topo=22080;
	 	dev="LO";
		res=IRA::CIRATools::tuneLocalOscillator(fs,fbw,lo,bs,bbw,topo,dev);
		ASSERT_TRUE(res);
		EXPECT_NEAR(lo,22512.0,TOLERANCE);
		EXPECT_NEAR(bs,400.0,TOLERANCE);
		
		fs=-375;
		fbw=275;
		bs=300;
		bbw=64;
	 	lo=21000;
	 	topo=22080;
	 	dev="ALL";
		res=IRA::CIRATools::tuneLocalOscillator(fs,fbw,lo,bs,bbw,topo,dev);
		ASSERT_TRUE(res);
		EXPECT_NEAR(lo,22592.5,TOLERANCE);
		EXPECT_NEAR(bs,480.5,TOLERANCE);	
		
	}
	
	void calculateIFLimits_WlowScenario() {
		double out_IF_min, out_IF_max , out_IF_start, out_IF_bw;
		bool result=IRA::CIRATools::calculateIFlimits(
		  96,
		  80,
		  98,
		  -1,
		  out_IF_min,out_IF_max,out_IF_start, out_IF_bw);
		ASSERT_TRUE(result);
		EXPECT_NEAR(out_IF_min,2.0,TOLERANCE);
		EXPECT_NEAR(out_IF_max,16.0,TOLERANCE); 
		EXPECT_NEAR(out_IF_start,-2.0,TOLERANCE);
		EXPECT_NEAR(out_IF_bw,14.0,TOLERANCE);
	}
			
	void dualConversion_TribandScenario() {
		
		// Case 1: the Wlow of the triband receiver
		// RF In: 80 - 98 GHz
		// OL1:   96 - 98 GHz (High Side -> -1)
		// IF1:   2 - 18 GHz
		// OL2:   3 - 8 GHz (High Side -> -1)
		// IF2:   0.375 - 0.650 GHz  (Pharos 2 derived band)
    
   		IRA::DoubleConversionAnalysis res = IRA::CIRATools::analyzeDualConversion(
       	 80.0, 98.0,       // RF Range Hardware
       	 96.0, 98.0,       // OL1 Range
       	 2.0, 18.0,        // IF1 Filter
       	 3.0, 8.0,         // OL2 Range
       	 0.375, 0.650,     // IF2 Filter
       	 -1, -1            // HSI, HSI
   		);
   
		ASSERT_TRUE(res.valid_configuration);
   
   		// Verifica OL Equivalente
   		// HSI: OL_eq = OL1 - OL2
   		// Min: 96 - 7 = 89
  		// Max: 98 - 7 = 91
   		EXPECT_NEAR(res.min_OL_eq, 88.0, TOLERANCE);
   		EXPECT_NEAR(res.max_OL_eq, 95.0, TOLERANCE);

   		// Verifica Inversione Spettro
   		// HSI + HSI = 2 inversioni = Diritto (false)
   		EXPECT_FALSE(res.spectrum_inverted);

    	// Verifica Banda RF Convertita
    	// Spettro Diritto: RF = OL_eq + IF2
    	// Min: 89.0 + 0.375 = 89.375
    	// Max: 91.0 + 0.650 = 91.650
    	EXPECT_NEAR(res.min_RF_converted, 88.375, TOLERANCE);
    	EXPECT_NEAR(res.max_RF_converted, 95.650, TOLERANCE);

    	// Verifica collo di bottiglia IF1
    	EXPECT_FALSE(res.limited_by_IF1);   
	}

	void dualConversion_FullLSI() {
		// --- TEST CASE 2: Low Side + Low Side (Standard) ---
		// Configurazione: LSI + LSI -> Spettro Diritto
    	IRA::DoubleConversionAnalysis res = IRA::CIRATools::analyzeDualConversion(
        5.0, 20.0,       // RF Hardware
        5.0, 6.0,         // OL1 Range
        1.0, 10.0,        // IF1 Filter (Largo)
        2.0, 3.0,         // OL2 Range
        0.5, 0.5,         // IF2 Filter (Stretto/Fisso)
        1, 1              // LSI, LSI
    	);

		// OL Eq = OL1 + OL2
		// Min Eq: 5 + 2 = 7
		// Max Eq: 6 + 3 = 9
		EXPECT_NEAR(res.min_OL_eq, 7.0, TOLERANCE);
		EXPECT_NEAR(res.max_OL_eq, 9.0, TOLERANCE);
		// RF = OL_eq + IF2 (Diritto)
		// Min RF: 7 + 0.5 = 7.5
		// Max RF: 9 + 0.5 = 9.5
		EXPECT_NEAR(res.min_RF_converted, 7.5, TOLERANCE);
		EXPECT_NEAR(res.max_RF_converted, 9.5, TOLERANCE);
    	EXPECT_FALSE(res.spectrum_inverted);
	}

	void dualConversion_SpectrumInversion() {
		// --- TEST CASE 3: Inversione Spettro (HSI + LSI) ---
		// Configurazione: Una sola conversione High Side -> Spettro Invertito
		IRA::DoubleConversionAnalysis res = IRA::CIRATools::analyzeDualConversion(
        10.0, 20.0,
        15.0, 15.0,       // OL1 Fisso (HSI)
        1.0, 6.0,         // IF1
        1.0, 1.0,         // OL2 Fisso (LSI)
        0.5, 1.0,         // IF2
        -1, 1             // HSI, LSI -> DISPARI -> Invertito
		);

		EXPECT_TRUE(res.spectrum_inverted);
    	// OL Eq (HSI base) = OL1 - OL2 = 15 - 1 = 14
		EXPECT_NEAR(res.min_OL_eq, 14.0, TOLERANCE);

		// Calcolo RF con Inversione
		// RF = OL_eq - IF2
		// Min RF (corrisponde a Max IF2): 14 - 1.0 = 13.0
		// Max RF (corrisponde a Min IF2): 14 - 0.5 = 13.5
		EXPECT_NEAR(res.min_RF_converted, 13.0, TOLERANCE);
		EXPECT_NEAR(res.max_RF_converted, 13.5, TOLERANCE);
	}
	
	void dualConversion_Bottleneck() {
	// --- TEST CASE 4: Verifica "Collo di Bottiglia" IF1 ---
	// Creiamo una situazione dove IF1 è troppo stretto e taglia il segnale
		IRA::DoubleConversionAnalysis res = IRA::CIRATools::analyzeDualConversion(
        80.0, 100.0,
        90.0, 90.0,       // OL1
        2.0, 3.0,         // IF1 (Molto stretto: 2-3 GHz)
        0.0, 0.0,         // OL2 (Non usato/zero per semplificare)
        3.5, 4.0,         // IF2 (Chiede frequenze tra 3.5 e 4.0 GHz)
        1, 1              // LSI
    );
    
    // Qui IF2 vorrebbe un segnale che in IF1 sarebbe a 3.5-4.0 GHz.
    // Ma IF1 lascia passare solo 2.0-3.0 GHz.
    // Risultato: Nessuna sovrapposizione valida.
    
    EXPECT_FALSE(res.valid_configuration);
    EXPECT_TRUE(res.limited_by_IF1); 
	}

	void cDualConversion_GeneralCase() {
   	// Input
    	double ol1 = 98.0;
    	double ol2 = 7.0;
    	double if2_min = 0.375;
    	double if2_max = 0.650;
    	double if1_limit_min = 2.0;
    	double if1_limit_max = 18.0;
    	long lo1_inj = -1; // HSI
    	long lo2_inj = -1; // HSI
		IRA::DoubleConversionResult res = IRA::CIRATools::calculateDualConversion(
        ol1, ol2, if2_min, if2_max, if1_limit_min, if1_limit_max, lo1_inj, lo2_inj
    	);

    	ASSERT_TRUE(res.valid) << "Configuration is valid!";
    
    	// Verifica Spettro: HSI + HSI = Diritto (Pari inversioni)
    	EXPECT_FALSE(res.spectrum_inverted); 

    	// Verifica OL Equivalente implicito: 98 - 7 = 91 GHz
    	// Verifica RF: Poiché Diritto -> RF = OL_eq + IF2
    	// Min: 91 + 0.375 = 91.375
    	// Max: 91 + 0.650 = 91.650
    	EXPECT_NEAR(res.rf_min_freq, 91.375, TOLERANCE);
    	EXPECT_NEAR(res.rf_max_freq, 91.650, TOLERANCE);
    	EXPECT_NEAR(res.rf_center_freq, (91.375 + 91.650)/2.0, TOLERANCE);

    	// Verifica IF1 Generata: HSI al 2° stadio -> IF1 = OL2 - IF2
    	// Min IF1 (generata da Max IF2): 7 - 0.650 = 6.350
    	// Max IF1 (generata da Min IF2): 7 - 0.375 = 6.625
    	EXPECT_NEAR(res.if1_min_generated, 6.350, TOLERANCE);
    	EXPECT_NEAR(res.if1_max_generated, 6.625, TOLERANCE);

    	// Verifica che IF1 sia dentro i limiti 2-18
    	EXPECT_GE(res.if1_min_generated, if1_limit_min);
    	EXPECT_LE(res.if1_max_generated, if1_limit_max);
	}

	void cDualConversion_IF1_Negative() {
   	double ol1 = 10.0;
		double ol2 = 1.0;  // OL2 molto basso
		double if2_min = 2.0; // IF2 più alta dell'OL2
		double if2_max = 3.0;
    	double if1_limit_min = 0.0; 
    	double if1_limit_max = 20.0;
    
    	// HSI al secondo stadio è cruciale per provocare la sottrazione
    	IRA::DoubleConversionResult res = IRA::CIRATools::calculateDualConversion(
        ol1, ol2, if2_min, if2_max, if1_limit_min, if1_limit_max, 1, -1 
    	);

    	EXPECT_FALSE(res.valid) << (const char *) res.error_msg;
	}	


	// Scenario: Le frequenze sono matematicamente valide (>0), ma cadono fuori dal filtro intermedio.
	// Esempio: Generiamo una IF1 a 1 GHz, ma il filtro parte da 2 GHz.
	void cDualConversion_IF1_OutOfBounds() {
		double ol1 = 98.0;
   	double ol2 = 0.5;
   	double if2_min = 0.4;
   	double if2_max = 0.6;
		double if1_limit_min = 2.0; 
		double if1_limit_max = 18.0;

    	// IF1 generata = 0.4 + 0.5 = 0.9 GHz. (Minore di 2.0 -> Errore)
    	IRA::DoubleConversionResult res = IRA::CIRATools::calculateDualConversion(
        ol1, ol2, if2_min, if2_max, if1_limit_min, if1_limit_max, -1, 1 
    	);
    	EXPECT_FALSE(res.valid) << (const char *) res.error_msg;
		// Debug info: verifichiamo che il calcolo fosse giusto ma respinto
		EXPECT_NEAR(res.if1_min_generated, 0.9, TOLERANCE);
	}

	::testing::AssertionResult skyFrequency_noIntersection() {
		double f,bw;
		double rf=100;
		double rbw=500;
		double bf=600;
		double bbw=300;
		::testing::Test::RecordProperty("description","check if the resulting sky frequency is actually empty");
		if (IRA::CIRATools::skyFrequency(bf,bbw,rf,rbw,f,bw)) {
			return ::testing::AssertionFailure() << "resulting band should be empty but bandwidth is " << bw;
		}
		rf=100;
		rbw=500;
		bf=-800;
		bbw=100;
		if (IRA::CIRATools::skyFrequency(bf,bbw,rf,rbw,f,bw)) {
			return ::testing::AssertionFailure() << "resulting band should be empty but bandwidth is " << bw;
		}
		else {
			return ::testing::AssertionSuccess();
		}
	}

	::testing::AssertionResult skyFrequency_intersection() {
		double f,bw;
		double rf=100;
		double rbw=200;
		double bf=200;
		double bbw=200;
		::testing::Test::RecordProperty("description","check (various data set) if the resulting sky frequency is not empty and band limits are correct");

		// R=100,200(100:300)U B=200,200(200:400)U => 200,100(200:300)
		if (IRA::CIRATools::skyFrequency(bf,bbw,rf,rbw,f,bw)) {
			if (f!=200) {
				return ::testing::AssertionFailure() << "1) straight receiver, straight backend, start frequency should be 200 but is " << f << " instead";
			}
			else if (bw!=100) {
				return ::testing::AssertionFailure() << "1) straight receiver, straight backend, start bandwidth should be 100 but is " << bw << " instead";
			}
		}
		else {
			return ::testing::AssertionFailure() << "1) straight receiver, straight backend, resulting band should not be empty" ;
		}
		// R=100,200(100:300)U B=150,50(150:200)U => 150,50(150:200)
		rf=100;
		rbw=200;
		bf=150;
		bbw=50;
		if (IRA::CIRATools::skyFrequency(bf,bbw,rf,rbw,f,bw)) {
			if (f!=bf) {
				return ::testing::AssertionFailure() << "2) straight receiver, straight backend, backend included, start frequency should be "
						<< bf << " but is " << f << " instead";
			}
			else if (bw!=bbw) {
				return ::testing::AssertionFailure() << "2) straight receiver, straight backend, backend included, start bandwidth should be "
						<< bbw << " but is " << bw << " instead";
			}
		}
		else {
			return ::testing::AssertionFailure() << "2) straight receiver, straight backend, backend included, resulting band should not be empty" ;
		}
		// R=100,200(100:300)U B=50,500(50:550)U => 100,200(100:300)
		rf=100;
		rbw=200;
		bf=50;
		bbw=500;
		if (IRA::CIRATools::skyFrequency(bf,bbw,rf,rbw,f,bw)) {
			if (f!=rf) {
				return ::testing::AssertionFailure() << "3) straight receiver, straight backend, receiver included, start frequency should be "
						<< rf << " but is " << f << " instead";
			}
			else if (bw!=rbw) {
				return ::testing::AssertionFailure() << "3) straight receiver, straight backend, receiver included, start bandwidth should be "
						<< rbw << " but is " << bw << " instead";
			}
		}
		else {
			return ::testing::AssertionFailure() << "3) straight receiver, straight backend, receiver included, resulting band should not be empty" ;
		}
		// R=-300,200(100:300)L B=200,100(200:300)U => -200,-100
		rf=-300;
		rbw=200;
		bf=200;
		bbw=100;
		if (IRA::CIRATools::skyFrequency(bf,bbw,rf,rbw,f,bw)) {
			if (f!=-200) {
				return ::testing::AssertionFailure() << "4) inverted receiver, straight backend, start frequency should be -200 but is " << f << " instead";
			}
			else if (bw!=-100) {
				return ::testing::AssertionFailure() << "4) inverted receiver, straight backend, start bandwidth should be -100 but is " << bw << " instead";
			}
		}
		else {
			return ::testing::AssertionFailure() << "4) inverted receiver, straight backend, resulting band should not be empty" ;
		}
		// R=-300,200(100:300)L B=100,100(100:300)U => -100,-100
		rf=-300;
		rbw=200;
		bf=100;
		bbw=100;
		if (IRA::CIRATools::skyFrequency(bf,bbw,rf,rbw,f,bw)) {
			if (f!=-100) {
				return ::testing::AssertionFailure() << "5) inverted receiver, straight backend, start frequency should be -200 but is " << f << " instead";
			}
			else if (bw!=-100) {
				return ::testing::AssertionFailure() << "5) inverted receiver, straight backend, start bandwidth should be -100 but is " << bw << " instead";
			}
		}
		else {
			return ::testing::AssertionFailure() << "5) inverted receiver, straight backend, resulting band should not be empty" ;
		}
		// R=-300,200(300:100)L B=150,50(150:200)U => -150,-50
		rf=-300;
		rbw=200;
		bf=150;
		bbw=50;
		if (IRA::CIRATools::skyFrequency(bf,bbw,rf,rbw,f,bw)) {
			if (f!=-bf) {
				return ::testing::AssertionFailure() << "6) inverted receiver, straight backend, backend included, start frequency should be "
						<< -bf << " but is " << f << " instead";
			}
			else if (bw!=-bbw) {
				return ::testing::AssertionFailure() << "6) inverted receiver, straight backend, backend included, start bandwidth should be "
						<< -bbw << " but is " << bw << " instead";
			}
		}
		else {
			return ::testing::AssertionFailure() << "6) inverted receiver, straight backend, backend included, resulting band should not be empty" ;
		}
		// R=-300,200(300:100)L B=50,500(50:550)U => -100,-200
		rf=-300;
		rbw=200;
		bf=50;
		bbw=500;
		if (IRA::CIRATools::skyFrequency(bf,bbw,rf,rbw,f,bw)) {
			if (f!=-100) {
				return ::testing::AssertionFailure() << "7) inverted receiver, straight backend, receiver included, start frequency should be -100"
						<< " but is " << f << " instead";
			}
			else if (bw!=-200) {
				return ::testing::AssertionFailure() << "7) inverted receiver, straight backend, receiver included, start bandwidth should be -200"
						<< " but is " << bw << " instead";
			}
		}
		else {
			return ::testing::AssertionFailure() << "7) inverted receiver, straight backend, receiver included, resulting band should not be empty" ;
		}
		// R=100,200(100:300)U B=-150,100(50:150)L => 150,-50
		rf=100;
		rbw=200;
		bf=-150;
		bbw=100;
		if (IRA::CIRATools::skyFrequency(bf,bbw,rf,rbw,f,bw)) {
			if (f!=150) {
				return ::testing::AssertionFailure() << "8) straight receiver, inverted backend, start frequency should be 150"
						<< " but is " << f << " instead";
			}
			else if (bw!=-50) {
				return ::testing::AssertionFailure() << "8) straight receiver, inverted backend, start bandwidth should be -50"
						<< " but is " << bw << " instead";
			}
		}
		else {
			return ::testing::AssertionFailure() << "8) straight receiver, inverted backend, resulting band should not be empty" ;
		}
		// R=-300,200(100:300)L B=-200,100(100:200)L => -200,100
		rf=-300;
		rbw=200;
		bf=-200;
		bbw=100;
		if (IRA::CIRATools::skyFrequency(bf,bbw,rf,rbw,f,bw)) {
			if (f!=-200) {
				return ::testing::AssertionFailure() << "9) inverted receiver, inverted backend, start frequency should be -200"
						<< " but is " << f << " instead";
			}
			else if (bw!=100) {
				return ::testing::AssertionFailure() << "9) inverted receiver, inverted backend, start bandwidth should be 100"
						<< " but is " << bw << " instead";
			}
		}
		else {
			return ::testing::AssertionFailure() << "9) inverted receiver, inverted backend, resulting band should not be empty" ;
		}
		// R=-300,200(100:300)L B=-200,100(100:200)L => -200,100
		rf=-1000;
		rbw=500;
		bf=50;
		bbw=2000;
		if (IRA::CIRATools::skyFrequency(bf,bbw,rf,rbw,f,bw)) {
			if (f!=-500) {
				return ::testing::AssertionFailure() << "10) inverted receiver, straight backend, start frequency should be -500"
						<< " but is " << f << " instead";
			}
			else if (bw!=-500) {
				return ::testing::AssertionFailure() << "10) inverted receiver, straight backend, start bandwidth should be -500"
						<< " but is " << bw << " instead";
			}
		}
		else {
			return ::testing::AssertionFailure() << "9) inverted receiver, inverted backend, resulting band should not be empty" ;
		}
		return ::testing::AssertionSuccess();
	}

	::testing::AssertionResult createEmptyFile_checkSuccess() {
		IRA::CString file;
		::testing::Test::RecordProperty("description","check the creation of an empty file");
		file=basePath+emptyFileName;
		if (IRA::CIRATools::createEmptyFile(file)) {
			if (IRA::CIRATools::fileExists(file)) {
				IRA::CString cleanCommand;
				cleanCommand="rm "+file;
				system((const char *)cleanCommand);
				return ::testing::AssertionSuccess();
			}
			else {
				return ::testing::AssertionFailure() << (const char *) file << " file should not be created but (fileExists) does not detect it";
			}
		}
		else {
			return ::testing::AssertionFailure() << (const char *) file << " file cannot be created";
		}
	}

	::testing::AssertionResult createEmptyFile_checkFail() {
		IRA::CString file;
		::testing::Test::RecordProperty("description","check if the creation fails when it is the case");
		file="/foo/boo/dummy/boogie.tst";
		if (!IRA::CIRATools::createEmptyFile(file)) {
			return ::testing::AssertionSuccess();
		}
		else {
			return ::testing::AssertionFailure() << (const char *) file << " does not fail as expected";
		}
	}

	::testing::AssertionResult deleteFile_checkSuccess() {
		IRA::CString file;
		IRA::CString command;
		::testing::Test::RecordProperty("description","check if a file can be deleted from filesystem");
		file=basePath+dummyFileName;
		command="touch "+file;
		system((const char *)command);
		if (IRA::CIRATools::deleteFile(file)) {
			return ::testing::AssertionSuccess();
		}
		else {
			return ::testing::AssertionFailure() << (const char *) file << " the file could not be deleted";
		}
	}

	::testing::AssertionResult extractFileName_checkSimpleDirectoryPath() {
		IRA::CString name;
		IRA::CString baseDir,baseName,extension;
		::testing::Test::RecordProperty("description","check is a fully qualified directory path can be decomposed correctly");
		// simple directory name
		name="/system/archive";
		if (!IRA::CIRATools::extractFileName(name,baseDir,baseName,extension)) {
			return ::testing::AssertionFailure() << "simple directory could not be decomposed";
		}
		else if (baseDir!="/system") {
			return ::testing::AssertionFailure() << "base directory of " << (const char *)name << " is unexpectedly " <<
					(const char *) baseDir;
		}
		else if (baseName!="archive") {
			return ::testing::AssertionFailure() << "base name of " << (const char *)name << " is unexpectedly " <<
					(const char *) baseName;
		}
		else if (extension!="") {
			return ::testing::AssertionFailure() << "extension token of " << (const char *)name << " is unexpectedly " <<
					(const char *) extension;
		}
		return ::testing::AssertionSuccess();
	}

	::testing::AssertionResult extractFileName_checkSimpleFilePath() {
		IRA::CString name;
		IRA::CString baseDir,baseName,extension;
		::testing::Test::RecordProperty("description","check is a fully qualified file path can be decomposed correctly");
		// simple directory name
		name="/system/archive/document.foo";
		if (!IRA::CIRATools::extractFileName(name,baseDir,baseName,extension)) {
			return ::testing::AssertionFailure() << "simple file path could not be decomposed";
		}
		else if (baseDir!="/system/archive") {
			return ::testing::AssertionFailure() << "base directory of " << (const char *)name << " is unexpectedly " <<
					(const char *) baseDir;
		}
		else if (baseName!="document") {
			return ::testing::AssertionFailure() << "base name of " << (const char *)name << " is unexpectedly " <<
					(const char *) baseName;
		}
		else if (extension!="foo") {
			return ::testing::AssertionFailure() << "extension token of " << (const char *)name << " is unexpectedly " <<
					(const char *) extension;
		}
		return ::testing::AssertionSuccess();
	}
	
	::testing::AssertionResult sendMail_checkSuccess() {
		IRA::CString sbj("test email");
		IRA::CString bdy("this is a test email. Trash it.");
		IRA::CString rcp("foo@mailserver.domain moo@m1.anotherdomain");
		::testing::Test::RecordProperty("description","check if an email can be sent");
		if (!IRA::CIRATools::sendMail(sbj,bdy,rcp,true)) {
			return ::testing::AssertionFailure(); 
		}
		return ::testing::AssertionSuccess();
	}


protected:
	static IRA::CString simpleDirPath;
	static IRA::CString complexDirPath;
	static IRA::CString fileName;
	static IRA::CString emptyFileName;
	static IRA::CString dummyFileName;
	IRA::CString basePath;

	static void TearDownTestCase()
	{
	}

	static void SetUpTestCase()
	{
	}

	virtual void SetUp() {
		IRA::CString command;
		char buff[256];
		getcwd(buff,256);
		basePath=buff;
		command="touch "+basePath+fileName;
		system((const char *)command);
	}
	virtual void TearDown() {
		IRA::CString cleanCommand;
		cleanCommand="rm -rf "+basePath+simpleDirPath;
		system((const char *)cleanCommand);
		cleanCommand="rm "+basePath+fileName;
		system((const char *)cleanCommand);
	}
};

IRA::CString IRALibrary_IRATools::simpleDirPath = IRA::CString("/firstLevelDirectory");
IRA::CString IRALibrary_IRATools::complexDirPath = simpleDirPath+IRA::CString("/secondLevelDirectory");
IRA::CString IRALibrary_IRATools::fileName = IRA::CString("/normalFile.tst");
IRA::CString IRALibrary_IRATools::emptyFileName = IRA::CString("/emptyFile.tst");
IRA::CString IRALibrary_IRATools::dummyFileName = IRA::CString("/dummyFile.tst");
}

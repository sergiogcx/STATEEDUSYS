<?php defined('SYSPATH') or die('No direct script access.');

/*
    FINAL EXAM CONTROLLER
    Sergio A. Garcia
    Last Update: 01/24/2011 11:18 PM

*/

/*
        NOTE: SOME PARTS OF THE CODE HAVE BEEN EDITED
*/


// Require the base Doctrine class
require_once ('bootstrap.php');
require_once ('config.php');

class Finalexam_Controller extends Controller
{
    private $instructionLanguage = 0;
    private $sessionPostId       = null;

    // Language Control
    private $languageModel       = null;

    function __construct() {
        parent::__construct();
        $this->session = Session::instance();
        $this->languageModel = new Translate_Model();
    }

    /**
     * Returns translation code.
     * @return integer
     */
    private function getTranslationCode()
    {
        return $this->languageModel->getLanguageMode();;
    }
	
    private function redirect($section = null) { url::redirect('https://www.statecertify.com/'.$section); }


    /**
     * Gets file from class number and $file name!
     * @param string $class
     * @param string $file
     */
    public function getFinalExam($class, $passcode, $file) {
            /******************************************
                            CODE EDITED OUT
            ******************************************/
    }
   
    /**
     * Shows the view with the good news!
     */
    public function userpass() {
            /******************************************
                            CODE EDITED OUT
            ******************************************/
    }

    /**
     * Shows the view with the good news!
     */
    public function userfailed() {
            /******************************************
                            CODE EDITED OUT
            ******************************************/
    }

    public function index($args = null, $mode = 0)
    {
        // 1) Authenticate or redirect to index
        // Done by initialize...

        // 2) Has user seen all modules / Lessons? Redirect Class
        if(!$exam->isPassed()) $exam->checkProgress();

        // 3) Make sure the user has 'done his time' in the course... or Redirect Home
        if(!$exam->isTimingValid()) $exam->checkTimeDifference();

        // 4) First Attempt?
        // TRUE: DISPLAY RULES
        // IS THIS A SECOND ATTEMPT:
        // TRUE: WARNING MESSAGE
        // FALSE: Redirect Home
        if(!$exam->isAttemptValid()) $exam->checkAttempts();

        // 5) Is Time Measured?
        // TRUE: INITIALIZE TIMER
        // INFORM USER
        if(!$exam->isFinalExamTimed()) $exam->isTimeMeasured();

        // 6) Set Time Stamp
        if(!$exam->isTimestampSet()) $exam->setTimeStamp();

        // 7) DISPLAY TEST
        // 1) SEKECT RANDOMLY A TEST FROM DB

        // Count Exams
        $numberOfFinalExams = $exam->class->Finalexam->count();
        $randomID = rand  (1, $numberOfFinalExams);

        // 2) DECRYPT TEST
        $examTitle = $exam->class->Finalexam[$randomID-1]->ExamTranslation[$this->getTranslationCode()]->title;
        $examXML = $exam->class->Finalexam[$randomID-1]->ExamTranslation[$this->getTranslationCode()]->xml;
        $exam->debug("Final Exams: " . $numberOfFinalExams . ", randomly selected: $randomID<br>");
        $exam->debug("EXAM TITLE: $examTitle<br>");
        $exam->debug("--------------------- PREPARING VIEW! ----------------------<br>");

        // 3) GENERATE PASSCODE WHICH WILL HELP AOIVD 'REFRESH' PROBLEMS
        $exam->allocatePass();

        // 4) PHRASE XML
        // 5) SHUFFLE ARRAY OF QUESTIONS/ANSWERS
        // 6) ECHO QUESTIONS ONLY FROM SUFFLED ARRAY
        // 7) IS TIME MEASURED
        // TRUE:	WAIT UNTIL TIME IS PU
        // FALSE: WAIT UNTIL SUBMISSION
        $view = new View($this->languageModel->getView('finalexam'));
        $view->pMenuContent     = $mode==0 ? $exam->getDebug() : 'The State requires that you finish the exam before you call us in case of any questions or additional support.<br><br>
                                                                  El Estado require que termine el examen antes de llamarnos en caso de cualquier duda o ayuda adicional.';
        $view->pClassHTML       = $exam->decode($examXML);
        $view->pIsPassed        = $exam->isPassed();
        $view->pIsTimingValid   = $exam->isTimingValid();
        $view->pIsAttemptValid  = $exam->isAttemptValid();
        $view->pAttemptsAllowed = $exam->getAttemptsAllowed();
        $view->pCurrentAttempts = $exam->getCurrentAttempts();

        $view->pExamPassCode    = $exam->getPassCode();
        $view->webstatsPkgID    = $this->urlEncode($randomID-1);
        $view->webstatsPkgLN    = $this->urlEncode($this->getTranslationCode());

        $view->pClassIsTimeMeasured = $exam->isFinalExamTimed();
        $view->render(TRUE);

    } // EOF Index

    /**
     * Obtains current referrer
     * @return string
     */
    private function getReferrer () {
        return request::referrer();
    }

    /**
     * Returns the final average calculation by input of points scored over
     * maximum possible points
     * @param int $pointsScored
     * @param int $possiblePoints
     * @return int
     */
    private function calculateQuizRate($pointsScored, $possiblePoints) {
        return ceil(((float)$pointsScored/$possiblePoints) * 100);
    }

    /**
     * It processes the incoming data from final exam and saves the results
     * into the database. Updates the user profile if fails or passes.
     * @return string
     */
    public function processResults() {

        // 1) Authenticate or redirect to index
        // Done by initialize...
        $exam = new Finalexam_Model;
        if(!$exam->isConnected()) $exam->initialize();

        // Check session or redirect...
        $passcode = $this->session->get('StateEducationalSessionPassCode', 'SES_NULL');
        $isValid = $exam->verifyPassCode($passcode);

        if(!$isValid) {
           die("Passcode NOT Valid!");
        }

	// XML Input with Results Report
	$xmlFinalExamSummary = $this->input->post("dr", null);

	// This should always be 100
	$finalMaximumScore = $this->input->post("tp", 100);

	// Get it, or gimme 0!
	$userFinalScore = $this->input->post("sp", 0);

	// Time to parse!
	$xml = new SimpleXMLElement($xmlFinalExamSummary);

	// Read the XML
	if($xmlFinalExamSummary !== null) {
		$examPercent = ceil((float)$xml->summary["percent"] * 100);
		$examScore = $xml->summary["score"];
		$examTime = $xml->summary["time"];
	} else {
		echo "Failure to communicate with server.";
		return false;
	}

        // 2) Has user seen all modules / Lessons?	Redirect Class
        if(!$exam->isPassed()) $exam->checkProgress();

        // 3) Check time difference 3:20 / 6:00		Redirect Home
        if(!$exam->isTimingValid()) $exam->checkTimeDifference();


        // 4) First Attempt?
                                // TRUE: DISPLAY RULES
                                // IS THIS A SECOND ATTEMPT:
                                                // TRUE: WARNING MESSAGE
                                                // FALSE: Redirect Home
        if(!$exam->isAttemptValid()) $exam->checkAttempts(true);

        // 5) Is Time Measured?
                                // TRUE: INITIALIZE TIMER
                                                        // INFORM USER
        if(!$exam->isFinalExamTimed()) $exam->isTimeMeasured();

	// Get Profile Id
	$profile_id = $exam->getProfileID();
	$examMinimalScore = $exam->class->finalscore;   // Minimal Score To Pass Class
        $certificateExpires = $exam->class->expires;    // Valid for how many years?

        // Let's calculate final average
        $psp_average = $this->calculateQuizRate($userFinalScore, $finalMaximumScore);

        // If they are the same allocate average into final variable...
        if($psp_average == $examPercent)
            $finalAverage = $examPercent;
        else
            die("Could not calculate final average...");

        // 7) IS SCORE >= 70 ?
        if($finalAverage >= $examMinimalScore) {
		// 1) UPDATE DATABSE & LOCK PROFILE
            	$exam->user->Profile[$profile_id]->finished = 1;	// Locked! ... yea right!
            	$exam->user->Profile[$profile_id]->finalscore = ceil($examPercent);
            	$exam->user->Profile[$profile_id]->dayfinished = $exam->encode(date('Y-m-d G:i:s'));

            	// 2) CLEAR PROFILE ADVANCEMENT
            	$exam->user->Profile[$profile_id]->currentmodule = 1;
            	$exam->user->Profile[$profile_id]->currentlesson = 1;
            	$exam->user->Profile[$profile_id]->lastmodule = 1;
            	$exam->user->Profile[$profile_id]->lastlesson = 1;
            	$exam->user->Profile[$profile_id]->timeout = $exam->encode(date('Y-m-d G:i:s'));
            	$exam->user->Profile[$profile_id]->sessionclass = $exam->encode(date('Y-m-d G:i:s'));
            	
            	// TABC Only
            	if($exam->user->Profile[$profile_id]->class_id == 1) {
					$popLicense = $exam->popLicense();
					$ready = $popLicense === "0" ? 0 : 1;
					
					$exam->user->Profile[$profile_id]->license = "" . $popLicense; // 0 Or not!
					$exam->user->Profile[$profile_id]->approved = $ready;	// depends on pop
					// $exam->user->Profile[$profile_id]->processed = $ready; 	// depends on pop
				}

            	// 3) WRITE EXPIRATION DATE!
            	$exam->user->Profile[$profile_id]->expires =
			$exam->encode(date("m/d/") . ((integer)(date("Y")+$certificateExpires)));

                // SAVE CHANGES!
            	$exam->user->save();
        }

	/*
	 *  AFTER IT IS ALL SAID AND DONE, THIS IS THE CULMINATION OF THIS FUNCTION
	 */
	echo "success";
	return true;
    }
    
    
	
	
    private function isAlpha($str) { return valid::alpha($str); }
    private function isNumeric($str) { return valid::numeric($str); }
    private function isAlphaDash($str) { return valid::alpha_dash($str); }
    private function isAlphaNum($str) { return valid::alpha_numeric($str); }
    private function isIP($str) { return valid::ip($str); }
    private function isDate($txt = '') {

        //
        // I need to check if this RegEx is secure, since it was generated by txt2re
        // but so far testing goes it all seems to be ok...
        //

        $re1='((?:(?:[1]{1}\\d{1}\\d{1}\\d{1})|(?:[2]{1}\\d{3}))[-:\\/.](?:[0]?[1-9]|[1][012])[-:\\/.](?:(?:[0-2]?\\d{1})|(?:[3][0,1]{1})))(?![\\d])';	# YYYYMMDD 1

        if ($c=preg_match_all ("/".$re1."/is", $txt, $matches)) {
            $yyyymmdd1=$matches[1][0];
            list($year, $month, $day) =  explode('-', $yyyymmdd1);
                return checkdate($month, $day, $year);
        }

            return false;
    }

    private function getDifference($str)
    {
        if($this->isDate($str))
        {
            list($a, $b) = explode(' ', $str);
            list($y, $m, $d) = explode('-', $a);
            list($hr, $min, $sec) = explode(':', $b);

            $date1 = time();
            $date2 = mktime($hr,$min,$sec,$m,$d,$y);

            $dateDiff = $date1 - $date2;
            $fullDays = floor($dateDiff/(60*60*24));
            $fullHours = floor(($dateDiff-($fullDays*60*60*24))/(60*60));
            $fullMinutes = floor(($dateDiff-($fullDays*60*60*24)-($fullHours*60*60))/60);
            $fullSeconds = floor(($dateDiff-($fullDays*60*60*24)-($fullHours*60*60))%60);

            return array($fullDays, $fullHours, $fullMinutes, $fullSeconds);
          }

      return null; // If the $str is not a date...
    }
	
    private function urlEncode($str)
    {
            $this->encrypt = new Encrypt();
            $output = null;
            while(!$this->isAlphaNum(substr($output, 0, strlen($output)-1))) { $output = $this->encrypt->encode($str); }
            return $output;
    } // EOF urlEncode
 	
}

?>

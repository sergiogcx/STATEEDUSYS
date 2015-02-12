<?php defined('SYSPATH') or die('No direct script access.');

class Controller_Welcome extends Controller {

	private $_key = "ses_livechat";
	private $_session = null;

	public function action_chatroom() {
		$id = Arr::get($_GET, 'id', "ses_empty");
		$passcode = Arr::get($_GET, 'tag', "ses_empty");
		$mode = Arr::get($_GET, 'mode', "ses_empty");
	
		if(!ctype_digit($id)) die('No input file specified (id).');
		if(!ctype_alnum($passcode)) die('No input file specified (tag).');
		
		$session = ORM::factory('session', $id);
		
		if($session->passcode === $passcode) {
			$view = View::factory('chatroom2');
				$view->ses_cnss_debug = false;
				$view->ses_cnss_id = $id;
				$view->ses_cnss_mode = $mode;
				$view->ses_cnss_passcode = $passcode;
				$view->ses_cnss_name = $session->name;
				$view->ses_cnss_time = $session->timestamp;
				$view->ses_cnss_sesid = $session->sesid;
				$view->ses_cnss_stauts = $session->status;
			$this->response->body($view);
		} else die('No input file specified (key).');
	}

	public function action_leavemessage() {
		$this->initializeSession();
		
		require_once ('securimage/securimage.php');

		$securimage = new Securimage();
		
		$vref = "https://www.statecertify.com/cnss/welcome";
		$vrsl = strlen($vref);
		$refx = substr($this->request->referrer(), 0, $vrsl);
		$bad = strcmp($vref, $refx) !== 0;
			   
		if($bad) {
			die('No input file specified (A).' . $this->request->referrer());
		}
		
		$ip = Arr::get($_SERVER, 'ip', "ses_empty");
		$email = Arr::get($_POST, 'email', "ses_empty");
		$name = Arr::get($_POST, 'name', "ses_empty");
		$message = Arr::get($_POST, 'message', "ses_empty");
		$hidden = Arr::get($_POST, 'hidden', "ses_empty");
		$captcha = Arr::get($_POST, 'captcha_code', "ses_empty");
		$errors_output = "";
		
		$errors = null;
		
		// Check referrer
		
		$post = Validation::factory($this->request->post());
		
		$post->rule('name', 'not_empty')
				->rule('name', 'regex', array(':value', '/^[a-zA-Z][a-zA-Z ]*$/'))
				->rule('name', 'max_length', array(':value','32'))
 
			// email
			->rule('email', 'not_empty')
			->rule('email', 'email')
			->rule('email', 'max_length', array(':value','64'))
			
			// message
			->rule('message', 'not_empty')
			->rule('message', 'max_length', array(':value','128'))
			
			//hidden
			->rule('hidden', 'max_length', array(':value','16'))
			->rule('hidden', 'alpha_numeric')
			
			// Captcha
			->rule('captcha_code', 'not_empty')
			->rule('captcha_code', 'alpha_numeric')
			->rule('captcha_code', 'max_length', array(':value','6'));
		
		$values = array($name, $email, $message, $hidden);
		
		if (!$post->check()) {
			// Validation failed, collect the errors
			$errors = $post->errors('user');
			
			
			foreach($errors as $error) {
				$errors_output .= $error . "<br/>";
			}
		}
		
		// check captcha
		if ($securimage->check($captcha) == false) {
			$errors_output .= "Invalid Captcha. Please try again." . "<br/>";
		}
		
		if(strlen($errors_output) === 0) {
			$headers = 'From: ' . $email . "\r\n" .
						'Reply-To: ' . $email . "\r\n" .
						'X-Mailer: PHP/' . phpversion();
						
			$sent = mail ("helpdesk@statecertify.com" , "Message Contact Box" , $message, $headers);
			mail("5127585487@tmomail.net", "Message Contact Box", $message, $headers);
			
			if($sent) {
				$securimage->createCode();
				$this->redirect('https://www.statecertify.com/cnss/welcome/messagesent', 302);
			}
		} else {
			$view = View::factory('leavemessage');
			
			$view->ses_cnss_debug = false;
			$view->ses_cnss_captcha = "";
			$view->ses_errors = $errors_output;
			$view->ses_values = $values;
			$this->response->body($view);
		}
	}
	
	public function action_startlivechat() {
		$this->initializeSession();
		
		require_once ('securimage/securimage.php');
		
		$securimage = new Securimage();
		
		$vref = "https://www.statecertify.com/cnss/welcome";
		$vrsl = strlen($vref);
		$refx = substr($this->request->referrer(), 0, $vrsl);
		$bad = strcmp($vref, $refx) !== 0;
			   
		if($bad) {
			die('No input file specified (B).' . $this->request->referrer());
		}

		$ip = Arr::get($_SERVER, 'ip', "ses_empty");
		$email = Arr::get($_POST, 'email', "ses_empty");
		$name = Arr::get($_POST, 'name', "ses_empty");
		$message = Arr::get($_POST, 'message', "ses_empty");
		$hidden = Arr::get($_POST, 'sesid', "ses_empty");
		$captcha = Arr::get($_POST, 'captcha_code', "ses_empty");
		$errors_output = "";
		
		$errors = null;
		
		// Check referrer
		
		$post = Validation::factory($this->request->post());
		
		$post->rule('name', 'not_empty')
				->rule('name', 'regex', array(':value', '/^[a-zA-Z][a-zA-Z ]*$/'))
				->rule('name', 'max_length', array(':value','32'))
 
			// email
			->rule('email', 'not_empty')
			->rule('email', 'email')
			->rule('email', 'max_length', array(':value','64'))
			
			// message
			->rule('message', 'not_empty')
			->rule('message', 'max_length', array(':value','128'))
			
			//hidden
			->rule('hidden', 'max_length', array(':value','16'))
			->rule('hidden', 'alpha_numeric')
			
			// Captcha
			->rule('captcha_code', 'not_empty')
			->rule('captcha_code', 'alpha_numeric')
			->rule('captcha_code', 'max_length', array(':value','6'));
		
		$values = array($name, $email, $message, $hidden);
		
		if (!$post->check()) {
			// Validation failed, collect the errors
			$errors = $post->errors('user');
			
			
			foreach($errors as $error) {
				$errors_output .= $error . "<br/>";
			}
		}
		
		// check captcha
		if ($securimage->check($captcha) == false) {
			$errors_output .= "Invalid Captcha. Please try again." . "<br/>";
		}
		
		if(strlen($errors_output) === 0) {
			$securimage->createCode();
			$existingSession = $this->checkExistingSession();
			$redirectId = 0;
			$redirectKey = 0;
			
				if($existingSession==null) {
					$nsession = $this->createNewSession($name, $email, $message, $hidden, 0); // Create Session
					$redirectId = $nsession[0];
					$redirectKey = $nsession[1];
				}
				else {
					$userializedSession = unserialize($existingSession);
					$redirectId = $userializedSession[0];
					$redirectKey = $userializedSession[1];
				}
			
			// $this->redirect('https://www.statecertify.com/cnss/welcome/chatroom?id=' . $nsession[0] . '&tag=' . $nsession[1], 302);	// send to chat...
			$this->redirect('https://www.statecertify.com/cnss/welcome/chatroom?id=' . $redirectId . '&tag=' . $redirectKey, 302);	// send to chat...
		} else {
			$view = View::factory('startchat');
			$view->ses_cnss_debug = false;
			$view->ses_cnss_captcha = "";
			$view->ses_errors = $errors_output;
			$view->ses_values = $values;
			$this->response->body($view);
		}
	}
	
	private function initializeSession() {
		$this->_session = Session::instance();
	}
	
	private function checkExistingSession() {
		return Cookie::get($this->_key, null);
	}
	
	public function action_messagesent() {
		$view = View::factory('messagesent');
		$view->ses_cnss_debug = false;
		$this->response->body($view);
	}
	
	
	public function action_operatorlogin() {
		$aip = "72.183.127.112";
		$aun = "sergiogcx";
		$apw = "9MS38lbac9i";
		
		$ip = Arr::get($_SERVER, 'REMOTE_ADDR', "ses_empty");
		$user = Arr::get($_POST, 'user', "ses_empty");
		$pass = Arr::get($_POST, 'pass', "ses_empty");
		$errors_output = "";
		
		$errors = null;
		
		// Check referrer
		
		$post = Validation::factory($this->request->post());
		
		$post->rule('pass', 'max_length', array(':value','16'))
			->rule('pass', 'alpha_numeric')
			
			->rule('user', 'max_length', array(':value','16'))
			->rule('user', 'alpha_numeric');
			
		if (!$post->check()) {
			// Validation failed, collect the errors
			$errors = $post->errors('user');
			
			
			foreach($errors as $error) {
				$errors_output .= $error . "<br/>";
			}
		}
		
		if(strcmp($ip, $aip) !== 0) $errors_output  .= "You are not welcome. ";
		
		if(strcmp($user, $aun) !== 0)
			$errors_output  .= "Bad Username, ";
		
		if(strcmp($pass, $apw) !== 0)
			$errors_output  .= "Bad Password";
			
			
		if(strlen($errors_output) !== 0) {
			$view = View::factory('operatorlogin');
			$view->ses_errors = $errors_output;
			$this->response->body($view);
		} else {
			// set session value
			$session = Session::instance();
			$session->set("ses_operator", $this->operatorcode());
			$this->redirect('https://www.statecertify.com/cnss/welcome/operator');
		}			
	}
	
	private function operatorcode() {
		return sha1(date("YdmG:i") . "5t4t3EDuC4t10na111C");
	}
	
	public function action_operatorlogout() {
		$session = Session::instance();
		$code = $session->delete("ses_operator");
		$this->redirect('https://www.statecertify.com/cnss/welcome/operatorlogin');
	}
	
	public function action_operator() {
		$session = Session::instance();
		$code = $session->get("ses_operator", "ses_error");
		
		if(strcmp($code, $this->operatorcode()) !== 0) {
			$this->redirect('https://www.statecertify.com/cnss/welcome/operatorlogin');
			return;
		}
		
		$view = View::factory('operator');
 
		$view->ses_cnss_debug = false;
	 
		// The view will have $places and $user variables
		$this->response->body($view);
	}


	public function action_index() {
		$this->initializeSession();
		$existingSession = $this->checkExistingSession();
		if($existingSession != null) {
			$data = unserialize($existingSession);
			$this->redirect('https://www.statecertify.com/cnss/welcome/chatroom?id=' . $data[0] . '&tag=' . $data[1] . '&mode=recovery', 302);	// send to chat...
			
		} else {
			$view = View::factory($this->isavailable() ? 'startchat' : 'leavemessage');
			$captcha = Captcha::instance();
			$view->ses_cnss_debug = false;
			$view->ses_cnss_captcha = "";
			$this->response->body($view);
		}
	}

	
	public function action_checkusers() {
		$session = ORM::factory('session');
		
		$sessions = $session->find_all();
		
		foreach($sessions as $user) {
			echo $user->id . "<br/>";
			echo $user->name . "<br/>";
			echo $user->email . "<br/>";
			echo $user->sesid . "<br/>";
			echo $user->status . "<br/>";
			echo "<br/>";
			echo "<br/>";
				
				
			$messages = $user->messages->find_all();
			$messages_number = $user->messages->count_all();
			echo "Messages: {$messages_number}<br/><br/>";
				
			foreach($messages as $message) {
				echo $message->id . "<br/>";
				echo $message->message . "<br/>";
			}
				
				
				
		}	

		echo "<hr/>";
		// foreach($sessions->messages as $message) { 	print_r($message); }
	}
	
	private function getnowdate() {
		return date("Y-d-m G:i:s");
	}
	
	private function newmessage($sessionid, $message_from, $message_str) {
		echo "newmessage: sessionid: $sessionid, message: $message_str<br/>";
		$session = ORM::factory('session', $sessionid);
		$message = ORM::factory('messages');
		$count = $session->count; 
		$message->sessionid = $sessionid;
		$message->from = $message_from;
		$message->message = $message_str;
		$message->timestamp = $this->getnowdate();
		$session->count = $count + 1;
		$session->save();
		$message->save();
	}
	
	private function validreferer() { 
		return true;
	}
	
	private function peel($str, $len = 0) {
		$strl = strlen($str);
		if($strl == 0) return;
		if($len >= $strl) $len = $strl;
		
		return substr($str, 0, $len);
	}
	
	private function cleanit($str) {
		return preg_replace("/[^A-Za-z0-9?![:space:]]/","",$str);
	}
	
	public function createNewSession($name, $email, $message = "", $sesid = 0, $status = 0) {
		$session = ORM::factory('session');
		$passcode = sha1("ses_" . uniqid() . "_cnss" ) ;		// Salted Passcode
		$ip = $_SERVER['REMOTE_ADDR']; // IP
		
		
		
		$session->name = $name;
		$session->email = $email;
		$session->status = $status;
		$session->sesid = $sesid;
		
		$session->passcode = $passcode;
		$session->ip = $ip;
		$session->timestamp = $this->getDateStamp();

		$session->save();
		
		// Welcome Message
		$this->newmessage($session->id,
						 "System",
						 "Welcome to StateCertify.com, please wait while we connect you to an operator.");
		
		$sessionData = array($session->id, $passcode, $email);
		Cookie::set($this->_key, serialize($sessionData));
						 
		return array($session->id, $passcode);
	}
	
	public function action_deletesession() {
		$sessionid = Arr::get($_GET, 'sessionid', 0);
		$session = ORM::factory('session', $sessionid);
		Cookie::delete($this->_key); // Delete cookie data first
		
		if($session->loaded()) {
			if($session->delete())
				echo "deleted";
			else
				echo "something went wrong...";
				
		}
		else {
			echo "Error: Session ID '{$sessionid}' Not Found";
		}
	}
	
	public function action_getnumberofmessages() {
		$sessionid = Arr::get($_GET, 'sessionid', 0);
		 
		if($sessionid > 0) {
			$session = ORM::factory('session', $sessionid);
			if($session->loaded()) {
				$messages_number = $session->count;
				echo $messages_number;
			} else {
				echo "0";
			}
		}
			
	}
	
	public function action_getmessages() {
		$sessionid = Arr::get($_GET, 'sessionid', 0);
		$difference = Arr::get($_GET, 'difference', 0);
		$mode = Arr::get($_GET, 'mode', 'ses_empty');

		$error = null;
		
		if($difference == 0)
			$error .= "System Error: No message difference reported.<br/>";
		
		if($sessionid == 0)
			$error .= "System Error: Session ID is not reported.<br/>";
			
		if(strlen($error)>0) {
			echo $error;
			return;
		}
		
		$order = ($mode === "recovery" ? "ASC" : "DESC");
		
		$records = DB::select()
					->from('messages')
					->where('sessionid','=', $sessionid)
					->order_by('id', $order)
					->limit($difference)
					->execute();
		
		foreach($records as $record) {
			echo $record['from'] . "|" . $record['message'] . "~";
		}
	}
	
	public function action_checklive() {
		$error = null;
		$sessionid = Arr::get($_GET, 'sessionid', 0);
		if($sessionid == 0)
			$error .= "System Error: Session ID is not reported.<br/>";
			
		if(strlen($error)>0) {
			echo $error;
			return;
		}
		
		$session = ORM::factory('session', $sessionid);
		if($session->loaded()) {
			echo $session->status;
			return;
		}
		
		echo "0";
	}
	
	public function action_postmessage() {
		$sessionid = Arr::get($_POST, 'sessionid', 0);
		$from = Arr::get($_POST, 'from', "ses_empty");
		$message = Arr::get($_POST, 'message', "ses_empty");
		
		if(strcmp($from, "ses_empty") == 0) {
			echo "Error, no (From)";
			return;
		}
		
		if(strcmp($message, "ses_empty") == 0) {
			echo "Error, no (Message)";
			return;
		}
		
		if($sessionid == 0) {
			echo "Error, no (sessionid)";
			return;
		}
		
		$this->newmessage($sessionid, $from,  $message);
	}
	
	public function action_getsessionsstatus() {
		$records = DB::select()
					->from('session')
					->order_by('id', 'DESC') 
					->execute();
		
		$usersCount = count($records);
		$highestId = 0;
		$userId = 0;
		
		$users = array();
		
		$n = 1;
		
		foreach($records as $user) {
			$userId = $user["id"];
			if($userId > $highestId) $highestId = $userId;
			$users[] = $userId . "|" . $user["name"] . "|" . $user["sesid"] . "|" . $user["status"] . "|". $user["count"] . ( $n == $usersCount? "" : "$" );	
			$n++;
		}
		
		// #split this first
		echo $usersCount . "#" . $highestId . "#";
		foreach($users as $user) echo $user;
		
	}
	
	public function action_getsessions() {
		$session = ORM::factory('session');
		$sessions = $session->find_all();
		
		foreach($sessions as $user) {
			echo $user->id . "|" . $user->name . "|" . $user->sesid . "|" . $user->status . "|". $user->count . "~";	
		}
	}
	
	
	
	public function action_downloadOperatorMessages() {
		$requestString = Arr::get($_POST, 'request', "ses_error");
		$output = "";
		
		if(strcmp($requestString, "ses_error")==0) { 
			echo ""; 
		}
		else {
			$users = explode("|", $requestString);
			foreach($users as $user) {
				if(strpos ( $user  , ":") > 0) {
					list($id, $difference) = explode(":", $user);
					$messages = $this->getmessages($id, $difference);
					$output .= substr($messages, 0, strlen($messages)-1) . "#";
				}
			}
			echo substr($output, 0, strlen($output)-1);
		}
	}
	
	
	public function getmessages($sessionid, $difference, $mode = 0) {
		$error = null;
		$output = "";
		if($difference == 0)
			$error .= "System Error: No message difference reported.<br/>";
		
		if($sessionid == 0)
			$error .= "System Error: Session ID is not reported.<br/>";
			
		if(strlen($error)>0) {
			echo $error;
			return;
		} 
		
		$records = DB::select()
					->from('messages')
					->where('sessionid','=', $sessionid)
					->order_by('id', 'DESC')
					->limit($difference)
					->execute();
		
		$output = "$sessionid*";
		foreach($records as $record) {
			$output .=  $record['from'] . "$" . $record['message'] . "*";
		}
		
		return $output;
	}
	
	public function action_activateUser() {
		$id = Arr::get($_POST, 'id', 0);
		
		if($id == 0) { 
			echo  "User not found."; 
			return;
		}
		
		$session = ORM::factory('session', $id);
		if($session->loaded()) {
			$session->status = 1;
			$session->save();
			echo "success";
			return;
		} 
		echo "User not found";
	}
	
	public function action_checkavailable() {
		// echo "current status: " . ($this->isavailable() ? "Available" : "Unavailable") . "<br/>";
		
		if($this->isavailable()) {
			$this->toggle_available_off();
		} else {
			$this->toggle_available_on();
		}
		
		echo ($this->isavailable() ? "available" : "unavailable");
	}
	
	private function isavailable() {
		$isavailable = file_get_contents($this->_availabilityfile);
		return strcmp($isavailable, "ses_available") == 0;
	}
	
	private function toggle_available_on() {
		file_put_contents($this->_availabilityfile, "ses_available");
	}
	
	private function toggle_available_off() {
		file_put_contents($this->_availabilityfile, "ses_not_available");
	}
	
	public function action_clearSessions() {
		// Get Sessions Count
		// if count == 0

			// Reset Sessions AUTOINCREMENT value
			// ALTER TABLE `session` AUTO_INCREMENT = 1;
			// Reset Messages AUTOINCREMENT value
			// ALTER TABLE `message` AUTO_INCREMENT = 1;
			// Print Success Message
		
		// else Print Error Message
		echo "Cleared";
	}
	
	public function action_handleuserexit() {
		$id = Arr::get($_GET, 'sessionid', "ses_empty");
		$passcode = Arr::get($_GET, 'tag', "ses_empty");
		$this->newmessage($id, "System", "ATTENTION: System: User has left.");
		echo "Message has been posted";
	}
	
	private function getDateStamp() {
		return date("Y-m-d G:i:s");
	}	
	
	
	
	
	
	public function action_mailSession() {
		$sessionid = Arr::get($_GET, 'sessionid', 0);
		$passcode = Arr::get($_GET, 'key', 0);
		$subject = "Live Chat Conversation Copy";
		
		if($sessionid == 0) die("ses_error_sesid.");
		if($passcode == 0) die("ses_error_passcode");
		
		$session = ORM::factory('session', $sessionid);
		
		if($session) {
			$dbpasscode = $session->passcode;
			$email = $session->email;
			
			if(strcmp($passcode, $dbpasscode) == 0) {
				$smail = new Model_Simplemail;
				$messages = $this->getMessagesForMail($sessionid);
				$body = "Live Chat Conversation Copy\n\r\n\r" . $messages;
				$smail->setUp($email, $subject, $body);
				$smail->sendmail();
				echo "ses_sent"; 
			}
		}
	}
	
	
	
	public function getMessagesForMail($sessionid) {
		$output = null;
		$records = DB::select()
					->from('messages')
					->where('sessionid','=', $sessionid)
					->execute();
		
		foreach($records as $record) {
			if(!strpos($record['message'], "User has left"))
			$output .= "From " . $record['from'] . ": " . $record['message'] . "\n\r";
		}
		
		return $output;
	}
	
	private function initializeSecurity() {
		$realm = 'Restricted Area';

		//user => password
		$users = array('sergiogcx' => '9MS38lbac9i', 'guest'=> 'guest');


		if (empty($_SERVER['PHP_AUTH_DIGEST'])) {
			header('HTTP/1.1 401 Unauthorized');
			header('WWW-Authenticate: Digest realm="'.$realm.
				   '",qop="auth",nonce="'.uniqid().'",opaque="'.md5($realm).'"');

			die('Unable to handle login.');
		}


		// analyze the PHP_AUTH_DIGEST variable
		if (!($data = http_digest_parse($_SERVER['PHP_AUTH_DIGEST'])) ||
			!isset($users[$data['username']]))
			die('Wrong Credentials!');


		// generate the valid response
		$A1 = md5($data['username'] . ':' . $realm . ':' . $users[$data['username']]);
		$A2 = md5($_SERVER['REQUEST_METHOD'].':'.$data['uri']);
		$valid_response = md5($A1.':'.$data['nonce'].':'.$data['nc'].':'.$data['cnonce'].':'.$data['qop'].':'.$A2);

		if ($data['response'] != $valid_response)
			die('Wrong Credentials!');

		


		// function to parse the http auth header
		function http_digest_parse($txt)
		{
			// protect against missing data
			$needed_parts = array('nonce'=>1, 'nc'=>1, 'cnonce'=>1, 'qop'=>1, 'username'=>1, 'uri'=>1, 'response'=>1);
			$data = array();
			$keys = implode('|', array_keys($needed_parts));

			preg_match_all('@(' . $keys . ')=(?:([\'"])([^\2]+?)\2|([^\s,]+))@', $txt, $matches, PREG_SET_ORDER);

			foreach ($matches as $m) {
				$data[$m[1]] = $m[3] ? $m[3] : $m[4];
				unset($needed_parts[$m[1]]);
			}

			return $needed_parts ? false : $data;
		}
	}
	

} // End Welcome

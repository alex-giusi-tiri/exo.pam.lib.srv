#pragma once

#include <iostream>
#include <sstream>

#include <zmq.hpp>

#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp> 

#include "server.hxx"

server::server (void)
{
	init ();
}

server::~server (void)
{
	fin ();
}

const bool server::init (void)
{
	reception = nullptr;
	_running = false;
	
	//context_publisher = new zmq::context_t (1);
	//context_subscriber = new zmq::context_t (1);
	context = new zmq::context_t (1);
	
	//socket_publisher = new zmq::socket_t (*context_publisher, ZMQ_PUB);
	//socket_subscriber = new zmq::socket_t (*context_subscriber, ZMQ_SUB);
	socket = new zmq::socket_t (*context, ZMQ_ROUTER);
	//socket = new zmq::socket_t (*context, ZMQ_DEALER);
	
	//socket_subscriber -> setsockopt (ZMQ_SUBSCRIBE, "", 0);
	
	//socket_publisher -> bind ("tcp://*:2131");
	//socket_subscriber -> connect ("tcp://0.0.0.0:2132");
	socket -> bind ("tcp://*:2231");
	//socket -> connect ("tcp://0.0.0.0:2132");
	
	return true;
}

const bool server::fin (void)
{
	stop ();
	
	//delete socket_subscriber;
	//delete socket_publisher;
	
	//zmq_close (socket);
	//zmq_ctx_destroy (context);
	
	//boost::this_thread::sleep (boost::posix_time::milliseconds (500));
	
	delete socket;
	
	//delete context_subscriber;
	//delete context_publisher;
	delete context;
	
	return true;
}

const bool server::transmit (const std::string & message, const bool & more) const
{
	std::cout << "server::transmit()" << std::endl;
	
	//  Prepare our context and publisher
	//zmq::context_t context (1);
	//zmq::socket_t socket (context, ZMQ_PUB);
	zmq::message_t zmq_message (message.size ());
	
	//socket.bind ("tcp://*:2131");
	
	//snprintf ((char *) zmq_message.data (), strlen (message.c_str ()) + 1, "%s", message.c_str ());
	//snprintf ((char *) zmq_message.data (), strlen (message.c_str ()) + 1, "%s", message.c_str ());
	memcpy (zmq_message.data (), message.data (), message.size ());
	//std::cout << "server::transmit()::zmq_message==[" << static_cast <const char *> (zmq_message.data ()) << ']' << std::endl;
	
	
	//if (!socket_publisher -> send (zmq_message))
	if (!socket -> send (zmq_message, more ? ZMQ_SNDMORE : 0))
	{
		std::cout << "server::transmit()::socket.send()==[false]" << std::endl;
		return false;
	}
	
	std::cout << "server::transmit()::socket.send()==[true]" << std::endl;
	return true;
}

const bool server::receive (std::string & message) const
{
	std::cout << "server::receive()" << std::endl;
	
	//zmq::context_t context (1);
	//zmq::socket_t socket (context, ZMQ_SUB);
	zmq::message_t zmq_message;
	
	//socket_subscriber -> connect ("tcp://0.0.0.0:2132");
	
	////if (socket.setsockopt (ZMQ_SUBSCRIBE, filter, strlen (filter)); != 0)
	//if (socket.setsockopt (ZMQ_SUBSCRIBE, filter, strlen (filter)); != 0)
	//	return false;
	//socket.setsockopt (ZMQ_SUBSCRIBE, "", 0);
	
	//if (!socket_subscriber -> recv (&zmq_message))
	if (!socket -> recv (&zmq_message))
	{
		std::cout << "server::receive()::socket.recv()==[false]" << std::endl;
		return false;
	}
	std::cout << "server::receive()::socket.recv()==[true]" << std::endl;
	//std::cout << "server::receive()::zmq_message==[" << static_cast <const char *> (zmq_message.data ()) << ']' << std::endl;
	//message = static_cast <const char *> (zmq_message.data ());
	message = std::string (static_cast <const char *> (zmq_message.data ()), zmq_message.size ());
//message = std::istringstream (static_cast <char *> (zmq_message.data ())).str ();
	std::cout << "server::receive()::message==[" << message << ']' << std::endl;
	
	return true;
}

void server::_receive (void)
{
	std::cout << "server::_receive()" << std::endl;
	
	std::string message;
	std::string identity;
	std::string query;
	noware::array <noware::array <>> result;
	noware::array <std::string, int> arguments;
	// Work-around
	//std::string size;
	//std::istringstream iss;
	
	while (_running)
	{
		result.clear ();
		arguments.clear ();
		
		std::cout << "server::_receive()::loop" << std::endl;
		
		
		// Receive the identity.
		std::cout << "server::_receive(identity)" << std::endl;
		if (!receive (identity))
		{
			std::cout << "server::_receive()::receive(identity)==[false]" << std::endl;
			
			// Try again.
			continue;
		}
		std::cout << "server::_receive()::receive(identity)==[" << identity << ']' << std::endl;
		
		std::cout << "server::_receive()::receive(1)" << std::endl;
		if (!receive (message))
		{
			std::cout << "server::_receive()::receive(1)==[false]" << std::endl;
			
			// Try again.
			continue;
		}
		std::cout << "server::_receive()::receive(1)::message==[" << message << ']' << std::endl;
		
		if (message == "get")
		{
			if (!receive (message))
			{
				std::cout << "server::_receive()::receive(get)==[false]" << std::endl;
				
				// Abort.
				continue;
			}
			std::cout << "server::_receive()::receive(get)==[true]" << std::endl;
			std::cout << "server::_receive()::receive(get)::message==[" << message << ']' << std::endl;
			
			if (message == "password")
			{
				/*
				if (!receive (message))
				{
					std::cout << "server::_receive()::rx(getter_type)==[false]" << std::endl;
					
					// Abort.
					continue;
				}
				std::cout << "server::_receive()::rx(getter_type)==[true]" << std::endl;
				std::cout << "server::_receive()::rx(getter_type)::message==[" << message << ']' << std::endl;
				
				if (message == "user")
				{
					if (!receive (message))
					{
						std::cout << "server::_receive()::rx(getter_type)==[false]" << std::endl;
						
						// Abort.
						continue;
					}
					std::cout << "server::_receive()::rx(getter_type)==[true]" << std::endl;
					std::cout << "server::_receive()::rx(getter_type)::message==[" << message << ']' << std::endl;
					
					
					if (message == "name")
					{
						*/
						if (!receive (message))
						{
							std::cout << "server::_receive()::receive(getter_content)==[false]" << std::endl;
							
							// Abort.
							continue;
						}
						std::cout << "server::_receive()::receive(getter_content)==[true]" << std::endl;
						std::cout << "server::_receive()::receive(getter_content)::message==[" << message << ']' << std::endl;
						
						//std::cout << "server::_receive()::receive(name)::message==[id]" << std::endl;
						
						// We do not need to use limit here, because user names must be unique,
						// so we should not get more than one user name.
						query = "select passwd from shadow where username = ?";
						arguments [1] = message;
						std::cout << "server::_receive()::arguments[1]==[" << arguments [1] << ']' << std::endl;
						
						// Send (like a 'set' operation) the peer identity first (who to send to).
						std::cout << "server::_receive()::transmit(identity)" << std::endl;
						if (!transmit (identity, true))
						{
							std::cout << "server::_receive()::transmit(identity)==[false]" << std::endl;
							
							// Abort.
							continue;
						}
						std::cout << "server::_receive()::transmit(identity)==[true]" << std::endl;
						
						if (!db.query (result, query, arguments))
						{
							std::cout << "server::_receive()::db.query()==[false]" << std::endl;
							// Send the success status (failed),
							// so the application would not continue requesting in the same session.
							if (!transmit ("0"))
							{
								std::cout << "server::_receive()::transmit(0)==[false]" << std::endl;
								
								// Abort.
								continue;
							}
						}
						else
						{
							// Nothing was found.
							if (result.empty ())
							{
								std::cout << "server::_receive()::result.empty()==[true]" << std::endl;
								
								// Announce that we have not found anything (user is absent).
								transmit ("-1");
								
								// Abort this request.
								continue;
							}
							
							// Send the success status (succeeded).
							if (!transmit ("1", true))
							{
								std::cout << "server::_receive()::transmit(1)==[false]" << std::endl;
								
								// Abort.
								continue;
							}
							
							// The order matters.
							
							// Password:
							std::cout << "server::_receive()::result[1][1](username)==[" << result [1] [1] << "]" << std::endl;
							if (!transmit (result [1] [1]))
							{
								std::cout << "server::_receive()::transmit(result[1])==[false]" << std::endl;
								
								// Abort.
								continue;
							}
						}
					}
					
					result.clear ();
					arguments.clear ();
				//}
			}
		else if (message == "set")
		{
			if (!receive (message))
			{
				std::cout << "server::_receive()::receive(get)==[false]" << std::endl;
				
				// Abort.
				continue;
			}
			std::cout << "server::_receive()::receive(get)==[true]" << std::endl;
			std::cout << "server::_receive()::receive(get)::message==[" << message << ']' << std::endl;
			
			if (message == "password")
			{
				/*
				if (!receive (message))
				{
					std::cout << "server::_receive()::rx(getter_type)==[false]" << std::endl;
					
					// Abort.
					continue;
				}
				std::cout << "server::_receive()::rx(getter_type)==[true]" << std::endl;
				std::cout << "server::_receive()::rx(getter_type)::message==[" << message << ']' << std::endl;
				
				if (message == "user")
				{
					if (!receive (message))
					{
						std::cout << "server::_receive()::rx(getter_type)==[false]" << std::endl;
						
						// Abort.
						continue;
					}
					std::cout << "server::_receive()::rx(getter_type)==[true]" << std::endl;
					std::cout << "server::_receive()::rx(getter_type)::message==[" << message << ']' << std::endl;
					
					
					if (message == "name")
					{
						*/
						// Get the user name.
						if (!receive (message))
						{
							std::cout << "server::_receive()::receive(getter_content)==[false]" << std::endl;
							
							// Abort.
							continue;
						}
						std::cout << "server::_receive()::receive(getter_content)==[true]" << std::endl;
						std::cout << "server::_receive()::receive(getter_content)::message==[" << message << ']' << std::endl;
						
						arguments [2] = message;
						
						
						// Get the password.
						if (!receive (message))
						{
							std::cout << "server::_receive()::receive(getter_content)==[false]" << std::endl;
							
							// Abort.
							continue;
						}
						std::cout << "server::_receive()::receive(getter_content)==[true]" << std::endl;
						std::cout << "server::_receive()::receive(getter_content)::message==[" << message << ']' << std::endl;
						
						arguments [1] = message;
						
						
						//std::cout << "server::_receive()::receive(name)::message==[id]" << std::endl;
						
						// We do not need to use limit here, because user names must be unique,
						// so we should not get more than one user name.
						query = "update Accounts set Password = ?1 where Name = ?2";
						
						std::cout << "server::_receive()::arguments[1]==[" << arguments [1] << ']' << std::endl;
						
						// Send (like a 'set' operation) the peer identity first (who to send to).
						std::cout << "server::_receive()::transmit(identity)" << std::endl;
						if (!transmit (identity, true))
						{
							std::cout << "server::_receive()::transmit(identity)==[false]" << std::endl;
							
							// Abort.
							continue;
						}
						std::cout << "server::_receive()::transmit(identity)==[true]" << std::endl;
						
						if (!db.query (result, query, arguments))
						{
							std::cout << "server::_receive()::db.query()==[false]" << std::endl;
							// Send the success status (failed),
							// so the application would not continue requesting in the same session.
							if (!transmit ("0"))
							{
								std::cout << "server::_receive()::transmit(0)==[false]" << std::endl;
								
								// Abort.
								continue;
							}
						}
						else
						{
							/*
							// Nothing was found.
							if (result.empty ())
							{
								std::cout << "server::_receive()::result.empty()==[true]" << std::endl;
								
								// Announce that we have not found anything (user is absent).
								transmit ("-1");
								
								// Abort this request.
								continue;
							}
							*/
							
							// Send the success status (succeeded).
							if (!transmit ("1"))
							{
								std::cout << "server::_receive()::transmit(1)==[false]" << std::endl;
								
								// Abort.
								continue;
							}
							
							// The order matters.
							/*
							// Password:
							std::cout << "server::_receive()::result[1][1](username)==[" << result [1] [1] << "]" << std::endl;
							if (!transmit (result [1] [1]))
							{
								std::cout << "server::_receive()::transmit(result[1])==[false]" << std::endl;
								
								// Abort.
								continue;
							}
							*/
						}
					//}
					
					result.clear ();
					arguments.clear ();
				//}
			}
		}
		
		std::cout << "server::_receive()::loop::end" << std::endl;
	}
}

const bool server::status (void) const
{
	return _running;
}

const bool server::stop (void)
{
	if (!status ())
		return true;
	
	if (reception != nullptr)
	{
		delete reception;
		reception = nullptr;
	}
	
	if (!db.disconnect ())
		return false;
	
	_running = false;
	
	return true;
}

const bool server::start (void)
{
	if (status ())
		return true;
	
	if (!db.connect ("/root/Projects/exo.nss.lib.srv/cfg/system.db"))
		return false;
	
	if (reception == nullptr)
		reception = new boost::thread (boost::bind (boost::mem_fn (&server::_receive), this));
	
	_running = true;
	
	return true;
}

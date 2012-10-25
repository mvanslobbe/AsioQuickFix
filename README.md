AsioQuickFix
============

AsioQuickFix is a replacement for the networking code that comes with QuickFIX. I built it because I use Boost heavily in my projects and I would not want to copy data between threads. 

Usage
=====

In the QuickFix examples, where one would usually do

FIX::SocketInitiator initiator ( application, storeFactory, fixSettings )
you now initiate
FIX::AsioSocketInitiator ( service, application, storeFactory, fixSettings )

Nothing else should change. Obviously, you want to make sure that your boost::asio::io_service is actually running and such.

Caveats
=======

At the moment, this is very much single threaded. If you try to run your io_service on multiple threads, you will not have a good time.

License
=======

Whatever makes the QuickFix people happy.



# Requirements
- OpenSSL
- quickFix
- Obviously, an API ID and Secret from the [Deribit Exchange test website](https://test.deribit.com)
- And most importantly, YouTube [link](https://youtu.be/j13iYc6zRuk?si=2N27C2yuHGdRlL4h), [link](https://youtu.be/QQzAHcojEKg?si=ygHRt7BhCz-kFHNb), and [link](https://youtu.be/qeH9Xv_90KM?si=WzvQEBPW6l0F8d6D) explaining in painful detail (each one more painfully explored than the last lol) how to include external dependencies in your Visual Studio solution. I struggled with that at first too.

# quickFix Tutorial
Ahem, well not exactly. There's lots of material online covering how to use the quickFix engine. There's the [documentation](https://quickfixengine.org/c/documentation/), and if you phrase your google prompts right, lots of example source code material. And that's not even including the example clients included in the quickFix engine open source code.

So, again, exhaustive.

So, **WHY SAY "TUTORIAL", MISTER?**

Fair expression, stay with me.

This, while not being an all-encompassing tutorial, will cover most of the problems someone new to quickFix (or navigating large codebases in general) might face and solutions that just work. It's not too large, hehe.

Fair warning, read this **after** studying the quickFix [documentation](https://quickfixengine.org/c/documentation/). Read this **only** if you still feel gaps in your understanding of how to make the damned client send messages right.

Basically, treat this as an extension of the already--again, exhaustive--quickFix documentation.

## Getting started
After including the quickFix engine source, you define a class that inherits the quickFix engine `Application` and `MessageCracker` classes. Read the [documentation](https://quickfixengine.org/c/documentation/) if this doesn't make sense.

The `Application` class contains a bunch of method declarations that need to be defined/implemented in your derived class. It's really that easy.

Now here's why I included this section: 
* `fromAdmin()` callback method gets automatically called when your client recieves an *administrative* level message from the server. Example administrative messages are: `Logon` and `Logout` messages.
* `toAdmin()` callback method gets automatically called when your client sends an *administrative* level message to the server. Same examples suffice.
* `fromApp()` callback method gets automatically called when your client recieves an *application* level message from the server. Example application message is a `Market Data Request` message.
* `toApp()` callback method gets automatically called when your client sends an *application* level message to the server. Same example suffices.

These callback method **(i.e you don't call them in your code, the quickFix engine does that for you--you can define what they do tho :))** could be used to log the correspondence between the client and the server as you (the developer) wishes.

I struggled with distinguishing these callback functions and figuring out when each was called. So there you have it, so you never have to.

## Logon
Your initiator should (and that's as compulsory as it gets) have an infinite loop in it's implementation of the `run()` method. Don't worry about it, it's something about concurrent programming within the quickFix engine. Breaking out of this `run()` method loop should be conditional.  

Besides that, the code here, contains a pretty good implemention of `toAdmin()` that handles logon messages correctly everytime.

## Maintaining sessions
Within your `onCreate()` method implementation, retrieve your sessionID and store it in a class variable. You'd need it passed as an argument to the `sendToTarget()` method. Without that, quickFix just can't identify the session within which the correspondence should happen.

## Recieving messages
Yes, you have `toApp()` implemented, yet you still can't get your server responses, even when you're SURE the server is up and responding. Been there. Implement an `onMessage()` method for each of the messages you'd be expecting from the server. Thank me later.

## That's about it...
Yep, that's pretty much it, those are the main problems I had to deal with working with quickFix and didn't have solutions readily available. Modify the code as you wish, and email me if you have any more problems wldfngrs@gmail.com. 

Enjoy messing with requests to/from the Deribit exchange.

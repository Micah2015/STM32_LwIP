/**************************************/
以太网实验，步骤

///
1.把M3接入路由。

///
2.ping实验。在接入相同路由的PC中，运行CMD，输入命令： ping 192.168.1.18 
  看是否能ping通，只有ping通后才能进行后续的实验。

*********ping通的实验现象如下（时间可能稍有区别）***********


	C:\Documents and Settings\>ping 192.168.1.18
	
	Pinging 192.168.1.18 with 32 bytes of data:
	
	Reply from 192.168.1.18: bytes=32 time=1ms TTL=255
	Reply from 192.168.1.18: bytes=32 time=2ms TTL=255
	Reply from 192.168.1.18: bytes=32 time<1ms TTL=255
	Reply from 192.168.1.18: bytes=32 time=1ms TTL=255
	
	Ping statistics for 192.168.1.18:
	    Packets: Sent = 4, Received = 4, Lost = 0 (0% loss),
	Approximate round trip times in milli-seconds:
	    Minimum = 0ms, Maximum = 2ms, Average = 1ms

******************************************************************
    若不能ping通，可能是路由的限制：
  	可尝试把M3与PC直接用网线相连， 再 ping 192.168.1.18  ，
	如果还不通，排除网线及接触问题后，可能是代码被修改或板子故障。

///
3.telnet实验。ping通后，输入命令: telnet 192.168.1.18

使用用户名：wildfire 密码：123456  登录。
登录成功后可选两条指令：LED1_ON 和 LED1_OFF 用于远程控制。

***************正常实验现象如下****************************************
屏幕输出		Please enter your username:

键盘输入		wildfire

屏幕输出		Please enter your password:

键盘输入		123456

屏幕输出		Please enter command:

键盘输入		LED1_ON
		
屏幕输出		 Success,LED1 status is ON!
		
屏幕输出		Please enter command:

键盘输入		LED1_OFF
		
屏幕输出		 Success,LED1 status is OFF!
		
屏幕输出		Please enter command:


************输入命令LED1_ON后，板上的LED1会点亮，输入LED1_OFF后，LED1熄灭****************
（win7系统没有telnet,需要在网上下载该程序进行实验）



////
4.浏览器实验。打开浏览器，在地址栏输入 http://192.168.1.18
  
  连接成功后，可看到登录界面。输入： 
*****************************************************  
用户名：wildfire
密码  ：123456
*****************************************************
登录成功后，可点击LED控制界面对M3板上的LED1进行控制。
（如果不是原版代码，该网页上的野火旗舰店网址链接可能会被恶意篡改。）

  



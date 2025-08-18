#pragma once

//动态多播 格式: 委托定义宏 (委托名,参数类型1,参数名1,参数类型2,参数名2)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDynamicMulticastDelegate, FString, str);

//创建动态多播对象
FDynamicMulticastDelegate DMOneParam;

class CallbackTarget
{
public:
	//动态多播回调函数必须使用UFUNCTION()
	UFUNCTION()
	void FunctionForAddDynamic(FString str);
};

class DynamicMultiCast
{
public:
	void Test()
	{
		CallbackTarget* Target=new CallbackTarget();
		//动态多播使用AddDynamic绑定回调函数
		DMOneParam.AddDynamic(Target,&CallbackTarget::FunctionForAddDynamic);
		
		//动态多播同样适用IsBound判断是否绑定
		DMOneParam.Broadcast("Dynamic Multicast Delegate Callback");

		/*
		 * NOTE: 解绑
		 */
		//动态多播使用Clear全部解绑，也支持使用Remove和Removeall,用法与多播一样
		DMOneParam.Clear();
		//动态多播使用RemoveDynamic()解绑单个
		DMOneParam.RemoveDynamic(Target, &CallbackTarget::FunctionForAddDynamic);
	}
};

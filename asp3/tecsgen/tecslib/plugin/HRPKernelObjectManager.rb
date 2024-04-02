#
#  TECS Generator
#      Generator for TOPPERS Embedded Component System
#  
#   Copyright (C) 2015-2021 by TOPPERS Project
#--
#   上記著作権者は，以下の(1)〜(4)の条件を満たす場合に限り，本ソフトウェ
#   ア（本ソフトウェアを改変したものを含む．以下同じ）を使用・複製・改
#   変・再配布（以下，利用と呼ぶ）することを無償で許諾する．
#   (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
#       権表示，この利用条件および下記の無保証規定が，そのままの形でソー
#       スコード中に含まれていること．
#   (2) 本ソフトウェアを，ライブラリ形式など，他のソフトウェア開発に使
#       用できる形で再配布する場合には，再配布に伴うドキュメント（利用
#       者マニュアルなど）に，上記の著作権表示，この利用条件および下記
#       の無保証規定を掲載すること．
#   (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使
#       用できない形で再配布する場合には，次のいずれかの条件を満たすこ
#       と．
#     (a) 再配布に伴うドキュメント（利用者マニュアルなど）に，上記の著
#         作権表示，この利用条件および下記の無保証規定を掲載すること．
#     (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに
#         報告すること．
#   (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
#       害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．
#       また，本ソフトウェアのユーザまたはエンドユーザからのいかなる理
#       由に基づく請求からも，上記著作権者およびTOPPERSプロジェクトを
#       免責すること．
#  
#   本ソフトウェアは，無保証で提供されているものである．上記著作権者お
#   よびTOPPERSプロジェクトは，本ソフトウェアに関して，特定の使用目的
#   に対する適合性も含めて，いかなる保証も行わない．また，本ソフトウェ
#   アの利用により直接的または間接的に生じたいかなる損害に関しても，そ
#   の責任を負わない．
#
#   $Id: HRPKernelObjectManager.rb 3225 2021-09-26 05:08:38Z okuma-top $
#  
#++
# This file has been generated by gen_kernel_celltype.py
class HRPKernelObjectManager
	@@kernel_objs = [\
		:tKernel, \
		:tTask, \
		:tSemaphore, \
		:tEventflag, \
		:tDataqueue, \
		:tPriorityDataqueue, \
		:tMutex, \
		:tFixedSizeMemoryPool, \
		:tTimeEventHandler, \
		:tCyclicNotifier, \
		:tAlarmNotifier, \
		:tInterruptRequest, \
		:tISR, \
		:tInterruptHandler, \
		:tCpuExceptionHandler, \
		:tInitializeRoutine, \
		:tTerminateRoutine, \
		:tMessageBuffer, \
	]

    def initialize()
        # 
        #  本クラスはインスタンスを持たない仮想的なクラスである
        #
        raise "class #{self.class.name} shall not have instances"
    end
    def self.include_celltype?(str)
        return @@kernel_objs.include?(str)
    end
end

class HRPSVCManage
    #
    #  @@id: TECSのプラグインで生成した拡張サービスコールに割り当てるid
    #        実際の拡張サービスコールIDは，TFN_TECSGEN_ORIGINで下駄を
    #        履かせた値となる
    #        TFN_TECSGEN_ORIGINは，hrp3/include/extsvc_fncode.h で定義
    #        される
    #  @@func_ids: 拡張サービスコールの関数名と拡張サービスコールIDを
    #  　　　対応づけるハッシュ
    #
    @@id = 0
    @@func_ids = {}
    def initialize()
        # 
      #  本クラスはインスタンスを持たない仮想的なクラスである
        #
        raise "class #{self.class.name} shall not have instances"
	end
	def self.add_cell node_root, cell_nsp, signature
		dbgPrint "HRPSVCManage: add_cell cell=#{cell_nsp.to_s} node_root=#{node_root.get_name} signature=#{signature.get_name}\n"
		if @@func_ids[ node_root ] == nil then
			@@func_ids[ node_root ] = {}
		end
		@@func_ids[ node_root ][ cell_nsp ] = signature
	end
	def self.get_list node_root
		if @@func_ids[ node_root ] == nil then
			@@func_ids[ node_root ] = {}
		end
		@@func_ids[ node_root ]
	end
end


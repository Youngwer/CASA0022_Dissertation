// components/ParametersGrid.js - 参数卡片网格组件（新布局）

import ParameterCard from './ParameterCard'

const ParametersGrid = ({ waterData }) => {
  // 只显示四个核心水质参数
  const coreParameters = [
    {
      icon: '⚗️',
      title: 'pH Level',
      value: waterData.ph,
      unit: '',
      param: 'ph',
      decimals: 1,
      description: 'pH值表示水的酸碱性，影响水的味道和安全性。理想饮用水pH值应在6.5-8.0之间，保证水质安全和口感。'
    },
    {
      icon: '🌫️',
      title: 'Turbidity',
      value: waterData.turbidity,
      unit: 'NTU',
      param: 'turbidity',
      decimals: 1,
      description: '浊度反映水中悬浮颗粒物的含量，影响水的透明度。低浊度（≤1.0 NTU）表示水质清澈，适合饮用。'
    },
    {
      icon: '⚡',
      title: 'Conductivity',
      value: waterData.conductivity,
      unit: 'μS/cm',
      param: 'conductivity',
      decimals: 1,
      description: '电导率反映水中离子总量，与溶解性固体含量相关。适宜饮用水电导率为100-400 μS/cm，保证适当的矿物质含量。'
    },
    {
      icon: '💎',
      title: 'TDS (Total Dissolved Solids)',
      value: waterData.tds,
      unit: 'ppm',
      param: 'tds',
      decimals: 1,
      description: 'TDS表示水中溶解的矿物质总量，影响水的味道和营养价值。理想TDS范围为80-300 ppm，既有益健康又口感良好。'
    }
  ]

  return (
    <div className="parameters-container">
      {/* 四个核心参数一行显示 */}
      <div className="core-params-row">
        {coreParameters.map((parameter) => (
          <ParameterCard
            key={parameter.param}
            icon={parameter.icon}
            title={parameter.title}
            value={parameter.value}
            unit={parameter.unit}
            param={parameter.param}
            decimals={parameter.decimals}
            description={parameter.description}
          />
        ))}
      </div>
    </div>
  )
}

export default ParametersGrid
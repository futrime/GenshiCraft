export default defineComponent(({ name, template, schema }) => {
	name('genshicraft:artifact')
	schema({
		description: 'The component for GenshiCraft artifacts.',
		properties: {
			rarity: {
				anyOf: [
					{
						enum: [1, 2, 3, 4, 5]
					}
				],
				description: 'The rarity of the artifact.',
				type: 'number'
			},
			series: {
				anyOf: [
					{
						enum: ['Adventurer']
					}],
				description: 'The set of the artifact.',
				type: 'string',
			},
			type: {
				anyOf: [
					{
						enum: ['Flower of Life', 'Plume of Death', 'Sands of Eon', 'Goblet of Eonothem', 'Circlet of Logos']
					}],
				description: 'The type of the artifact.',
				type: 'string',
			}
		},
		required: ['rarity', 'series', 'type']
	})

	template(({ rarity, series, type }, { create }) => {
		create(
			{
				'minecraft:creative_category': {
					'parent': `itemGroup.name.GenshiCraft.Artifacts.${series}.${type}.${rarity.toString()}`
				},
				'minecraft:max_stack_size': 1,
				'minecraft:render_offsets': {
					'main_hand': {
						'first_person': {
							'scale': [
								0.003,
								0.003,
								0.003
							]
						},
						'third_person': {
							'scale': [
								0.006,
								0.006,
								0.006
							]
						}
					}
				},
			},
			'minecraft:item/components'
		)

		if (type == 'Circlet of Logos') {
			create(
				{
					"minecraft:allow_off_hand": true,
					"minecraft:hand_equipped": true,
					'minecraft:render_offsets': {
						'off_hand': {
							'first_person': {
								'scale': [
									0.,
									0.,
									0.
								]
							},
							'third_person': {
								'scale': [
									0.,
									0.,
									0.
								]
							}
						}
					}
				},
				'minecraft:item/components'
			)
		} else {
			create(
				{
					'minecraft:armor': {
						'protection': 0
					}
				},
				'minecraft:item/components'
			)
		}

		switch (type) {
			case 'Flower of Life':
				create(
					{
						'minecraft:wearable': {
							"dispensable": true,
							"slot": "slot.armor.head"
						}
					},
					'minecraft:item/components'
				)
				break
			case 'Plume of Death':
				create(
					{
						'minecraft:wearable': {
							"dispensable": true,
							"slot": "slot.armor.chest"
						}
					},
					'minecraft:item/components'
				)
				break
			case 'Sands of Eon':
				create(
					{
						'minecraft:wearable': {
							"dispensable": true,
							"slot": "slot.armor.legs"
						}
					},
					'minecraft:item/components'
				)
				break
			case 'Goblet of Eonothem':
				create(
					{
						'minecraft:wearable': {
							"dispensable": true,
							"slot": "slot.armor.feet"
						}
					},
					'minecraft:item/components'
				)
				break
			case 'Circlet of Logos':
				create(
					{
						'minecraft:wearable': {
							"dispensable": true,
							"slot": "slot.weapon.offhand"
						}
					},
					'minecraft:item/components'
				)
				break
		}
	})
})

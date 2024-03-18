export default defineComponent(({ name, template, schema }) => {
	name('genshicraft:weapon')
	schema({
		description: 'The component for GenshiCraft weapons.',
		properties: {
			rarity: {
				anyOf: [
					{
						enum: [1, 2, 3, 4, 5]
					}
				],
				description: 'The rarity of the weapon.',
				type: 'number'
			},
			type: {
				anyOf: [
					{
						enum: ['Sword', 'Claymore', 'Polearm', 'Catalyst', 'Bow']
					}
				],
				description: 'The type of the weapon.',
				type: 'string',
			}
		},
		required: ['rarity', 'type']
	})

	template(({ rarity, type }, { create }) => {
		create(
			{
				'minecraft:creative_category': {
					'parent': `itemGroup.name.GenshiCraft.Weapons.${type}.${rarity.toString()}`
				},
				'minecraft:damage': 0,
				'minecraft:hand_equipped': true,
				'minecraft:max_stack_size': 1,
				'minecraft:weapon': {}
			},
			'minecraft:item/components'
		)

		switch (type) {
			case 'Sword':
				create(
					{
						'minecraft:render_offsets': {
							'main_hand': {
								'first_person': {
									'scale': [
										0.003,
										0.001,
										0.003
									]
								},
								'third_person': {
									'position': [
										0.2,
										1.1,
										-0.4
									],
									'scale': [
										0.006,
										0.001,
										0.006
									]
								}
							}
						}
					},
					'minecraft:item/components'
				)
				break
		}
	})
})
